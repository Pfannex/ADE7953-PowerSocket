#include "FFS.h"

//###############################################################################
//  FFS
//###############################################################################

FFS::FFS(I2C& i2c):
    i2c(i2c),
  //jsonFiles
    cfg(CFG_PATH, TYPE_OBJECT),
    sub(SUB_PATH, TYPE_OBJECT),
    subGlobal(SUB_GLOBAL_PATH, TYPE_OBJECT),
    pub(PUB_PATH, TYPE_OBJECT),
    ade7953(ADE7953_PATH, TYPE_OBJECT),

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
    subGlobal.loadFile();
    sub.loadFile();
    pub.loadFile();
    ade7953.loadFile();
    myFile.loadFile();
    Serial.println("............................................");
  }
}

//...............................................................................
//  load string from file system
//...............................................................................

String FFS::loadString(String filePath) {

  FFSstringFile FS(filePath);
  FS.loadFile();
  return FS.read();
}

//...............................................................................
//  TEST
//...............................................................................
void FFS::TEST(){
  Serial.println(cfg.readItem("apName"));
  Serial.println(cfg.readItem("wifiSSID"));
  Serial.println(cfg.readItem("wifiPSK"));
  Serial.println(cfg.readItem("mqttServer"));
  Serial.println(cfg.size);

  cfg.writeItem("apName", "ESP8266_1032096");
  cfg.writeItem("wifiSSID", "Pf@nne-NET");
  cfg.writeItem("wifiPSK", "Pf@nneNETwlan_ACCESS");
  cfg.writeItem("mqttServer", "192.168.1.203");
  cfg.saveFile();

  Serial.println(cfg.readItem("apName"));
  Serial.println(cfg.readItem("wifiSSID"));
  Serial.println(cfg.readItem("wifiPSK"));
  Serial.println(cfg.readItem("mqttServer"));
  Serial.println(cfg.size);

  i2c.lcd.println(sub.readItem("1.0"), ArialMT_Plain_16, 0);
  i2c.lcd.println(cfg.readItem("wifiSSID"), ArialMT_Plain_16, 16);
  i2c.lcd.println(cfg.readItem("wifiPSK"), ArialMT_Plain_16, 24);
  i2c.lcd.println(cfg.readItem("mqttServer"), ArialMT_Plain_16, 32);
}

//-------------------------------------------------------------------------------
//  FFS API
//-------------------------------------------------------------------------------
void FFS::set(TTopic topic){
/*
FFS
  └─FILE
     ├─filePath                     R
     ├─size                         R
     ├─itemsCount                   R
     ├─type                         R
     ├─root                         RW
     ├─loadFile()
     ├─readItem(itemName)
     ├─writeItem(itemName, value)
     └─saveFile()
*/

Serial.println(cfg.readItem("webUser"));

  if (topic.item[2] == "cfg"){
    if (topic.item[3] == "item"){
      cfg.writeItem(topic.item[4], topic.arg[0]);   //write Item
    }else if (topic.item[3] == "saveFile"){         //save File
      cfg.saveFile();
    }

  }else if (topic.item[1] == "ade7953"){
       //call FFS subAPI
       //ffs.set(topic);
  }

  Serial.println(cfg.readItem("webUser"));





}


String FFS::get(TTopic topic){

}

//-------------------------------------------------------------------------------
//  FFS private
//-------------------------------------------------------------------------------

//###############################################################################
//  FFSstringFile
//###############################################################################

FFSstringFile::FFSstringFile(String filePath):
  filePath(filePath) {
}

//-------------------------------------------------------------------------------
//  FFSstringFile public
//-------------------------------------------------------------------------------

//...............................................................................
//  load string from FFS file
//...............................................................................

void FFSstringFile::loadFile() {

  File stringFile;

  Serial.print("reading " + filePath + "... ");
  if (SPIFFS.exists(filePath)) {
    stringFile = SPIFFS.open(filePath, "r");
    if (stringFile) {
      Serial.println("OK");
      data= stringFile.readString();
      stringFile.close();
    } else {
      Serial.println("ERROR (open)");
      data= "";
    }
  } else {
      Serial.println("ERROR (no such file)");
  }
}

//...............................................................................
//  get string
//...............................................................................

String FFSstringFile::read() {

  return data;
}

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
//  save root string to FFS-File (for external use)
//...............................................................................
bool FFSjsonFile::saveFile(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);

  File jsonFile = SPIFFS.open(filePath, "w");
  if (!jsonFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  json.printTo(jsonFile);
  jsonFile.close();
  return true;
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

  root = "";           //printTo(String) is additive!!
  json.printTo(root);
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
  String jsonData;

  Serial.print("reading " + filePath + "...");
  if (SPIFFS.exists(filePath)) {
    jsonFile = SPIFFS.open(filePath, "r");
    if (jsonFile) {
      Serial.println("OK");
      size = jsonFile.size();

      DynamicJsonBuffer jsonBuffer;
      JsonVariant json = jsonBuffer.parse(jsonFile);
      if (json.is<JsonArray>()) {
        JsonArray& arr = json.as<JsonArray>();
        json.printTo(jsonData);
      }
      if (json.is<JsonObject>()) {
        JsonObject& obj = json.as<JsonObject>();
          json.printTo(jsonData);
          itemsCount = parseJsonObject(obj);
      }
    }else{
      Serial.println("ERROR openFile");
      return "NIL";
    }
    jsonFile.close();
  }
  return jsonData;
}

//...............................................................................
//  parse jsonObject recursively
//...............................................................................
int FFSjsonFile::parseJsonObject(JsonObject& jsonObject){
  int count = 0;
  //Serial.println("parseJsonObject");
  for (auto &element : jsonObject){
    String strKey = element.key;
    String strValue = element.value;
    //Serial.print(strKey);Serial.print("      ");Serial.println(strValue);
    count ++;
  }
  return count;
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
