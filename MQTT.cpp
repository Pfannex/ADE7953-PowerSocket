#include "MQTT.h"

MQTT::MQTT(){
  
}

//===> get pubTopic <-------------------------------------------------
String MQTT::pubTopic(String item){
  return getTopic(item, pub_root);
}
//===> get subTopic <-------------------------------------------------
String MQTT::subTopic(String item){
  return getTopic(item, sub_root);
}

//===> get Topic from Object <----------------------------------------
String MQTT::getTopic(String item, JsonObject& root){
  String topic = "NIL";
  for (auto &element : root){
    String strKey = element.key;
    if (strKey == item){
      String strValue = element.value;
      topic = strValue;
      break;
    }
  }
  return topic;
}

//xxxxxxxx===> getFileSize <---------------------------------------------------
size_t MQTT::getFileSize(String path){
  File tmpFile;
  size_t size = 0;
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("");
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists(path)) {
      Serial.println("file exists");
      tmpFile = SPIFFS.open(path, "r");
      if (tmpFile) {
        Serial.println("opened File");
        size = tmpFile.size();
      }
    }
  }
  return size;
}

//xxxxx===> Read json DATA <------------------------------------------------
String MQTT::FileReadjsonData(String path){
 File jsonFile;
 String jsonData = "NIL";
 
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("");
  Serial.println("mounting FS...for jsonFile");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists(path)) {
      //file exists, reading and loading
      Serial.println("file exists jsonFile");
      jsonFile = SPIFFS.open(path, "r");
      if (jsonFile) {
        Serial.println("opened jsonFile");
        size_t size = jsonFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        jsonFile.readBytes(buf.get(), size);
        
        //Array
        /*
        DynamicJsonBuffer jsonBufferArray;
        JsonArray& jsonArray = jsonBufferArray.parseArray(buf.get());             
        if (jsonArray.success()) {
          //Serial.println("inside reading");
          //Serial.println(jsonArray[0].asString());
          char buffer[size];
          jsonArray.printTo(buffer, sizeof(buffer));
          jsonData = String(buffer);
      
        }else{
          Serial.println("failed to load  ARRAY");
        }*/
        
        //Object
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        if (json.success()) {         
          char buffer[size];
          json.printTo(buffer, sizeof(buffer));
          jsonData = String(buffer);
        }else{
          Serial.println("failed to load OBJECT");
        }
      }
    }else{
      Serial.println("jsonFile does not exist");
    }
  }else{
    Serial.println("failed to mount FS");
  }
  jsonFile.close();
  //end read  
  return jsonData;  
}

//===> parse json DATA ARRAY <------------------------------------------------
void MQTT::parseJSONarray(JsonArray& jsonArray){
//Serial.println("##### parseJSONarray ####");  
  int i=0;
  for (auto &element : jsonArray){
    if (element.is<JsonArray&>()){
      Serial.println("-->>");//Serial.println(i);
      JsonArray& nestedArray = element;
      parseJSONarray(nestedArray);
    }else{
      Serial.print(element.asString()); Serial.print(" | ");Serial.println(i);
    }
    i++;      
  }
}
//===> parse json DATA OBJECT <------------------------------------------------
void MQTT::parseJSONobject(JsonObject& jsonObject){
//Serial.println("##### parseJSONobject ####");  

  for (auto &element : jsonObject){
    String strKey = element.key;
    String strValue = element.value;
    Serial.print(strKey);Serial.print("      ");Serial.println(strValue);
  }
}
