#include "FFS.h"

//###############################################################################
//  FFS 
//###############################################################################

FFS::FFS(I2C& i2c):
    i2c(i2c),
  //jsonFiles
    cfg(CFG_PATH, TYPE_OBJECT),
    myFile(MYFILE_PATH, TYPE_OBJECT),
    testArray(TESTARRAY_PATH, TYPE_ARRAY){
}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------

//...............................................................................
//  Mount FFS
//...............................................................................
void FFS::mount(){

  Serial.print("Mounting FS...");
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
  }else{
    Serial.println("OK");
    //Serial.print("formating FS...");
    //SPIFFS.format();
    //Serial.println("OK");
    
  //load rootStrings
    cfg.loadFile();
    myFile.loadFile();

  }
  
//--- TEST----------------------------------------  
  Serial.println(cfg.readItem("apName"));
  Serial.println(cfg.readItem("wifiSSID"));
  Serial.println(cfg.readItem("wifiPSK"));
  Serial.println(cfg.readItem("mqttServer"));
  Serial.println(cfg.size);

  cfg.writeItem("apName", "ESP8266_1032096");
  cfg.writeItem("wifiSSID", "Pf@nne-NET");
  cfg.writeItem("wifiPSK", "Pf@nneNETwlan_ACCESS");
  cfg.writeItem("mqttServer", "192.168.1.203");
  
  Serial.println(cfg.readItem("apName"));
  Serial.println(cfg.readItem("wifiSSID"));
  Serial.println(cfg.readItem("wifiPSK"));
  Serial.println(cfg.readItem("mqttServer"));
  Serial.println(cfg.size);
  
  i2c.lcd.println(cfg.readItem("apName"), 0);
  i2c.lcd.println(cfg.readItem("wifiSSID"), 1);
  i2c.lcd.println(cfg.readItem("wifiPSK"), 2);
  i2c.lcd.println(cfg.readItem("mqttServer"), 3);
}

//-------------------------------------------------------------------------------
//  FFS private
//-------------------------------------------------------------------------------

//###############################################################################
//  FFSjsonFile
//###############################################################################

FFSjsonFile::FFSjsonFile(String filePath, int type):
    filePath(filePath),
    type(type){
}

//-------------------------------------------------------------------------------
//  FFSjsonFile public
//-------------------------------------------------------------------------------

//...............................................................................
//  load root string from FFS-File (for external use)
//...............................................................................
void FFSjsonFile::loadFile(){
  root = readJsonString();
}

//...............................................................................
//  read Item from jsonObjectString
//...............................................................................
String FFSjsonFile::readItem(String itemName){
  DynamicJsonBuffer JsonBuffer;
  JsonObject& rootObject = JsonBuffer.parseObject(root);  
  return rootObject[itemName].asString();
}

//...............................................................................
//  read Item from jsonArrayString
//...............................................................................
String FFSjsonFile::readItem(int item){
  DynamicJsonBuffer JsonBuffer;
  JsonArray& rootArray = JsonBuffer.parseArray(root);   
  return rootArray[item].asString();
}

//...............................................................................
//  writeItem to jsonObjectString
//...............................................................................
bool FFSjsonFile::writeItem(String itemName, String value){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);

  json[itemName] = value;
  
  File jsonFile = SPIFFS.open(filePath, "w");
  if (!jsonFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  json.printTo(jsonFile);
  jsonFile.close(); 
  root = readJsonString();
  return true;
}

//-------------------------------------------------------------------------------
//  FFSjsonFile private
//-------------------------------------------------------------------------------

//...............................................................................
//  read json-root-string from File
//...............................................................................
String FFSjsonFile::readJsonString(){  
    
  File jsonFile;
  String jsonData = "NIL";
 
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
          jsonData = String(buffer);
        }
      }
    }
  }
  jsonFile.close();
  return jsonData;  
}

//...............................................................................
//  parse jsonObject recursively
//...............................................................................
void FFSjsonFile::parseJsonObject(JsonObject& jsonObject){
  //Serial.println("parseJsonObject");
  for (auto &element : jsonObject){
    String strKey = element.key;
    String strValue = element.value;
    Serial.print(strKey);Serial.print("      ");Serial.println(strValue);
  }
}

//...............................................................................
//  parse jsonArray recursively
//...............................................................................
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





















