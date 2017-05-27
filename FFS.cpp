#include "FFS.h"

//===============================================================================
//  FFS 
//===============================================================================
FFS::FFS(I2C& i2c):
    i2c(i2c),
    //jsonFiles
    cfg(CFG_PATH, TYPE_OBJECT),
    myFile(MYFILE_PATH, TYPE_OBJECT),
    testArray(TESTARRAY_PATH, TYPE_ARRAY){
}

//  FFS public
//===============================================================================

//===> mount FFS <-------------------------------------------------
void FFS::mount(){

  Serial.print("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }else{
    Serial.println("OK");
    //Serial.print("formating FS...");
    //SPIFFS.format();
    //Serial.println("OK");
    
    //cfg.loadFile();
    //myFile.loadFile();

  cfg.loadFile();
  Serial.println("cfg.root: " + cfg.root);
  
  cfg.read("apName");
  cfg.write("apName", "my World");
  cfg.read("apName");
  
  cfg.loadFile();
  Serial.println(cfg.root);
    
  }
  
//--- TEST----------------------------------------  



//--- TEST----------------------------------------  
//for (int i=0; i<100; i++){ 
  //Serial.println(i); 
  //Serial.println(cfg.readItem("apName"));
  //Serial.println(cfg.readItem("wifiSSID"));
  //Serial.println(cfg.readItem("wifiPSK"));
  //Serial.println(cfg.readItem("mqttServer"));
  
  //Serial.println(myFile.readItem("Field_01"));
  //Serial.println(myFile.readItem("Field_02"));
  //myFile.writeItem("","");

//----------- 

/*
  DynamicJsonBuffer JsonBuffer;
  JsonObject& rootObject = JsonBuffer.parseObject(myFile.root);  
  rootObject["Field_01"] = "Hello";
  rootObject["Field_02"] = "World!";

  File configFile = SPIFFS.open("/MyFile.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    //return false;
  }

  rootObject.printTo(configFile);*/
 

//-----------
  //Serial.println(myFile.readItem("Field_01"));
  //Serial.println(myFile.readItem("Field_02"));
//}  

  
  //i2c.lcd.println(myFile.readItem("Field_01"), 0);
  //i2c.lcd.println(myFile.readItem("Field_02"), 1);
 
  //i2c.lcd.println(cfg.readItem("wifiPSK"), 2);
  //i2c.lcd.println(cfg.readItem("mqttServer"), 3);
  //cfg.writeItem("wifiSSID", "HelloWorld!");
  //i2c.lcd.println(cfg.readItem("wifiSSID"), 3);
  //Serial.println(cfg.readItem("wifiSSID"));

}
//  FFS private
//===============================================================================

//===============================================================================
//  FFSjsonFile
//===============================================================================

FFSjsonFile::FFSjsonFile(String filePath, int type):
    filePath       (filePath),
    type           (type){
    //root           (readJsonString()){
    //jsonRootObject (JsonBuffer.parseObject(root)),
    //jsonRootArray  (JsonBuffer.parseArray(root)){
}

//###############################################################################
//  TEST
//###############################################################################

//===> read <-------------------------------------------------
bool FFSjsonFile::read(String itemName){
  File configFile = SPIFFS.open(filePath, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  //StaticJsonBuffer<200> jsonBuffer;
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* val = json[itemName];
  //const char* accessToken = json["wifiSSID"];

  // Real world application would store these values in some variables for
  // later use.

  //Serial.print("Loaded serverName: ");
  Serial.println(val);
  //Serial.print("Loaded accessToken: ");
  //Serial.println(accessToken);
  return true;
    
}

//===> write <-------------------------------------------------
bool FFSjsonFile::write(String itemName, String value){
  //char rootjson[] = "{\"apName\":\"x5x5x5\",\"apPassword\":\"ESP8266config\"}";
  
  //StaticJsonBuffer<200> jsonBuffer;
  DynamicJsonBuffer jsonBuffer;
  //JsonObject& json = jsonBuffer.createObject();
  //JsonObject& json = jsonBuffer.parseObject(rootjson);
  JsonObject& json = jsonBuffer.parseObject(root);
  json[itemName] = value;

  File configFile = SPIFFS.open(filePath, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  Serial.print("printTo: "); json.printTo(Serial); Serial.println("");
  return true;
  
}

//###############################################################################
//  TEST
//###############################################################################




//  FFSjsonFile public
//===============================================================================

//===> loadFile <-----------------------------------------------------
void FFSjsonFile::loadFile(){
  root = readJsonString();
}

//===> readItem from jsonString <-------------------------------------
String FFSjsonFile::readItem(String itemName){
  DynamicJsonBuffer JsonBuffer;
  JsonObject& rootObject = JsonBuffer.parseObject(root);  
  Serial.println(""); 
  parseJsonObject(rootObject);  
  return rootObject[itemName].asString();
}

String FFSjsonFile::readItem(int item){
  DynamicJsonBuffer JsonBuffer;
  JsonArray& rootArray = JsonBuffer.parseArray(root);   
  parseJsonArray(rootArray); 
  Serial.println(""); 
  return rootArray[item].asString();
}

//===> writeItem to jsonString <---------------------------------------
bool FFSjsonFile::writeItem(String itemName, String value){

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["Field_01"] = "Hello";
  json["Field_02"] = "World!";

  File configFile = SPIFFS.open(MYFILE_PATH, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;

  
/*  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  json[itemName] = value;

  File jsonFile = SPIFFS.open(filePath, "w");
  if (!jsonFile) {
    Serial.println("failed to open File for writing");
    //Serial.print("format file System.. ");
    //SPIFFS.format();
  }

  json.printTo(Serial);
  json.printTo(jsonFile);
  jsonFile.close(); 
*/
}

//  FFSjsonFile private 
//===============================================================================

//===> Read json String from File <------------------------------------
String FFSjsonFile::readJsonString(){  
    
  File jsonFile;
  String jsonData = "NIL";
 
  //if (SPIFFS.begin()) {
    Serial.print("reading " + filePath + "...");
    if (SPIFFS.exists(filePath)) {
      jsonFile = SPIFFS.open(filePath, "r");
      if (jsonFile) {
        Serial.println("OK");
        size = jsonFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        jsonFile.readBytes(buf.get(), size);
        if (type == TYPE_ARRAY){
        //Array 
          DynamicJsonBuffer jsonBufferArray;
          JsonArray& jsonArray = jsonBufferArray.parseArray(buf.get());             
          if (jsonArray.success()) {
            char buffer[size];
            jsonArray.printTo(buffer, sizeof(buffer));
            jsonData = String(buffer); 
          }
        }else{
        //Object
          DynamicJsonBuffer jsonBuffer;
          JsonObject& json = jsonBuffer.parseObject(buf.get());
          if (json.success()) {         
            char buffer[size+1];
            json.printTo(buffer, sizeof(buffer));
            Serial.print("printTo readJsonString: ");
            json.printTo(Serial);
            Serial.println("");
            jsonData = String(buffer);
          }
        }
      }
    }
  //}
  jsonFile.close();
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
  for (auto &element : jsonArray){
    if (element.is<JsonArray&>()){
      Serial.println("-->>");//Serial.println(i);
      JsonArray& nestedArray = element;
      parseJsonArray(nestedArray);
    }else{
      Serial.println(element.asString()); //Serial.print(" | ");Serial.println(i);
    }
  }
}





















