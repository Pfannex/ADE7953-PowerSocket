#include "FFS.h"

//===============================================================================
//  FFS 
//===============================================================================
FFS::FFS(I2C& i2c):
    i2c(i2c),
    cfg(CFG_PATH, TYPE_OBJECT),
    testArray(TESTARRAY_PATH, TYPE_ARRAY){

}

//===> TEST <------------------------------------------------------
void FFS::TEST(){
  

  Serial.println(cfg.readItem("apName"));
  Serial.println(cfg.readItem("wifiSSID"));
  Serial.println(cfg.readItem("wifiPSK"));
  Serial.println(testArray.readItem(1));

  cfg.writeItem("wifiSSID", "HelloWorld");  //Pf@nne-NET
  //Serial.println(cfg.readItem("wifiSSID"));

  //i2c.lcd.println(cfg.readItem("WiFiSSID"), 0);
  i2c.lcd.println(String(cfg.size), 1);
  
  //Serial.println("TEST");
  //Serial.println(CFG_root);
  //Serial.println(getObjectItem(CFG_root, "wifiSSID"));
  //Serial.println(getArrayItem(testArray_root, 0));
}
  

//===============================================================================
//  FFSjsonFile public
//===============================================================================
FFSjsonFile::FFSjsonFile(String filePath, int type) :
    filePath       (filePath),
    type           (type),
    root           (readJsonString()),
    jsonRootObject (JsonBuffer.parseObject(root)),
    jsonRootArray  (JsonBuffer.parseArray(root)){
}

//===> readItem from jsonString <-------------------------------------
String FFSjsonFile::readItem(String itemName){
  //DynamicJsonBuffer JsonBuffer;
  //JsonObject& rootObject = JsonBuffer.parseObject(root);  
  //return rootObject[itemName].asString();
  
  return jsonRootObject[itemName].asString();
}

String FFSjsonFile::readItem(int item){
  //DynamicJsonBuffer JsonBuffer;
  //JsonArray& rootArray = JsonBuffer.parseArray(root);  
  //return rootArray[item].asString();
  return jsonRootArray[item].asString();
  
}

//===> writeItem to jsonString <---------------------------------------
void FFSjsonFile::writeItem(String itemName, String value){
  jsonRootObject[itemName] = value;

  //SPIFFS.begin();
  File jsonFile = SPIFFS.open(filePath, "w");
  if (!jsonFile) {
    Serial.println("failed to open File for writing");
    //Serial.print("format file System.. ");
    //SPIFFS.format();
  }

  jsonRootObject.printTo(Serial);
  jsonRootObject.printTo(jsonFile);
  jsonFile.close(); 

/*  
  DynamicJsonBuffer JsonBuffer;
  JsonObject& rootObject = JsonBuffer.parseObject(root);  
  rootObject[itemName] = value;

  //SPIFFS.begin();
  File jsonFile = SPIFFS.open(filePath, "w");
  if (!jsonFile) {
    Serial.println("failed to open File for writing");
    //Serial.print("format file System.. ");
    //SPIFFS.format();
  }

  rootObject.printTo(Serial);
  rootObject.printTo(jsonFile);
  jsonFile.close(); 
*/  
}

//  FFSjsonFile private 
//===============================================================================

//===> Read json String from File <------------------------------------
String FFSjsonFile::readJsonString(){    
  //Serial.println("FFSjsonFile::readJsonString()");
  File jsonFile;
  String jsonData = "NIL";
 
  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("");
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system...OK");
    if (SPIFFS.exists(filePath)) {
      //file exists, reading and loading
      Serial.print("file exists: ");Serial.println(filePath);
      jsonFile = SPIFFS.open(filePath, "r");
      if (jsonFile) {
        Serial.println("opened File...OK");
        size = jsonFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        jsonFile.readBytes(buf.get(), size);
        
        if (type == TYPE_ARRAY){
        //Array 
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
          }
        }else{
        //Object
          DynamicJsonBuffer _jsonBuffer;
          JsonObject& json = _jsonBuffer.parseObject(buf.get());
          if (json.success()) {         
            char buffer[size];
            json.printTo(buffer, sizeof(buffer));
            jsonData = String(buffer);
          }else{
            Serial.println("failed to load OBJECT");
          }
        }
      }
    }else{
      Serial.println("jsonFile does not exist");
    }
  }else{
    Serial.println("failed to mount FS");
  }
  Serial.println("");
  jsonFile.close();
  //end read    
  //Serial.println(jsonData);    
  return jsonData;  
}





//#########################################################################################





void FFSjsonFile::parseJsonObject(JsonObject& jsonObject){
  //Serial.println("parseJsonObject");
  for (auto &element : jsonObject){
    String strKey = element.key;
    String strValue = element.value;
    Serial.print(strKey);Serial.print("      ");Serial.println(strValue);
  }
}

void FFSjsonFile::parseJsonArray(JsonArray& jsonArray){
  //int i=0;
  for (auto &element : jsonArray){
    if (element.is<JsonArray&>()){
      Serial.println("-->>");//Serial.println(i);
      JsonArray& nestedArray = element;
      parseJsonArray(nestedArray);
    }else{
      Serial.println(element.asString()); //Serial.print(" | ");Serial.println(i);
    }
    //i++;      
  }
}





















