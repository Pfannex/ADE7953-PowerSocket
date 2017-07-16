#include "FFS.h"

//###############################################################################
//  FFS
//###############################################################################

FFS::FFS(SysUtils& sysUtils):
    sysUtils(sysUtils),
    //i2c(i2c),
  //jsonFiles
    cfg(CFG_PATH, TYPE_OBJECT),
    sub(SUB_PATH, TYPE_OBJECT),
    subGlobal(SUB_GLOBAL_PATH, TYPE_OBJECT),
    pub(PUB_PATH, TYPE_OBJECT),
    ade7953(ADE7953_PATH, TYPE_OBJECT){
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

    Serial.println("FFS::mount()");

  //load rootStrings
    cfg.loadFile();
    subGlobal.loadFile();
    sub.loadFile();
    pub.loadFile();
    ade7953.loadFile();
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
//  check for valid JSON-String
//...............................................................................
bool FFS::isValidJson(String root){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  return json.success();
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

/*  i2c.lcd.println(sub.readItem("1.0"), ArialMT_Plain_16, 0);
  i2c.lcd.println(cfg.readItem("wifiSSID"), ArialMT_Plain_16, 16);
  i2c.lcd.println(cfg.readItem("wifiPSK"), ArialMT_Plain_16, 24);
  i2c.lcd.println(cfg.readItem("mqttServer"), ArialMT_Plain_16, 32);*/
}

//-------------------------------------------------------------------------------
//  FFS API
//-------------------------------------------------------------------------------
//...............................................................................
//  API SET
//...............................................................................
/*
ffs
  └─fileObject
      ├─mount
      ├─loadFile
      ├─saveFile
      ├─root          [jsonString]    RW
      └─item
          └─itemName  [value]         RW
*/
String FFS::set(Topic& topic){
  String str = "NIL";

  FFSjsonFile *tmpFile = NULL;
  if (topic.itemIs(3, "cfg")) {
    tmpFile = &cfg;
  }else if (topic.itemIs(3, "sub")){
    tmpFile = &sub;
  }else if (topic.itemIs(3, "subGlobal")){
    tmpFile = &subGlobal;
  }else if (topic.itemIs(3, "pub")){
    tmpFile = &pub;
  }else if (topic.itemIs(3, "ade7953")){
    tmpFile = &ade7953;
  }

  if (tmpFile != NULL){
//LoadFile
    if (topic.itemIs(4, "loadFile")){
      tmpFile->loadFile();
      return "OK";
//SaveFile
    }else if (topic.itemIs(4, "saveFile")){
      tmpFile->saveFile();
      return "OK";
//write rootString
    }else if (topic.itemIs(4, "root")){
      if (topic.countArgs > 0){
        if (isValidJson(topic.arg[0])){
          tmpFile->root = topic.arg[0];
          return "OK";
        }else{
          sysUtils.logging.error("no valid JSON-String!");
          return "no valid JSON-String!";
        }
      }else{
        sysUtils.logging.error("Argument not found!");
        return "Argument not found!";
      }

//writeItem
    }else if (topic.itemIs(4, "item")){
      if (topic.countItems > 5 and topic.countArgs > 0){
        tmpFile->writeItem(topic.item[5], topic.arg[0]);
        return tmpFile->readItem(topic.item[5]);
      }else{
        sysUtils.logging.error("Topic or Argument not found!");
        return "Topic or Argument not found!";
      }
    }
//ERROR--------------------------------------------------
  }else{
    sysUtils.logging.error("No match file found!");
    return "No match file found!";
  }
}

//...............................................................................
//  API GET
//...............................................................................
/*
ffs
  └─fileObject
     ├─filePath     R
     ├─size         R
     ├─itemsCount   R
     ├─type         R
     ├─root         RW
     └─Item
         └─ItemName RW
*/
String FFS::get(Topic& topic){
  FFSjsonFile *tmpFile = NULL;
  if (topic.itemIs(3, "cfg")) {
    tmpFile = &cfg;
  }else if (topic.itemIs(3, "sub")){
    tmpFile = &sub;
  }else if (topic.itemIs(3, "subGlobal")){
    tmpFile = &subGlobal;
  }else if (topic.itemIs(3, "pub")){
    tmpFile = &pub;
  }else if (topic.itemIs(3, "ade7953")){
    tmpFile = &ade7953;
  }

  if (tmpFile != NULL){
//filePath
    if (topic.itemIs(4, "filePath")){
      return tmpFile->filePath;
//FileSize
    }else if (topic.itemIs(4, "size")){
      return String(tmpFile->size);
//ItemsCount
    }else if (topic.itemIs(4, "itemsCount")){
      return String(tmpFile->itemsCount);
//Json Object Type
    }else if (topic.itemIs(4, "type")){
      return String(tmpFile->type);
//write rootString
    }else if (topic.itemIs(4, "root")){
      return tmpFile->root;
//readItem
    }else if (topic.itemIs(4, "item")){
      return tmpFile->readItem(topic.item[5]);
    }
//ERROR
  }else{
    sysUtils.logging.error("No match file found!");
    return "NIL";
  }
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
bool FFSjsonFile::loadFile(){
  root = readJsonString();
  if (root = "NIL")
    return false;
  else
    return true;
}

//...............................................................................
//  save root string to FFS-File (for external use)
//...............................................................................
bool FFSjsonFile::saveFile(){
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  if (json.success()) {
    File jsonFile = SPIFFS.open(filePath, "w");
    if (!jsonFile) {
      Serial.println("Failed to open config file for writing");
      return false;
    }
    json.printTo(jsonFile);
    jsonFile.close();
    return true;
  }else{
    return false;
  }
}

//...............................................................................
//  read Item from jsonObjectString
//...............................................................................
String FFSjsonFile::readItem(String itemName){
  DynamicJsonBuffer JsonBuffer;
  JsonObject& rootObject = JsonBuffer.parseObject(root);
  if (rootObject.success()) {
    return rootObject[itemName].asString();
  }else{
    return "NIL";
  }
}

//...............................................................................
//  read Item from jsonArrayString
//...............................................................................
String FFSjsonFile::readItem(int item){
  DynamicJsonBuffer JsonBuffer;
  JsonArray& rootArray = JsonBuffer.parseArray(root);
  if (rootArray.success()) {
    return rootArray[item].asString();
  }else{
    return "NIL";
  }
}

//...............................................................................
//  writeItem to jsonObjectString
//...............................................................................
bool FFSjsonFile::writeItem(String itemName, String value){
  //Serial.println(itemName + ":" + value);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(root);
  if (json.success()){
    //json.printTo(Serial);
    json[itemName] = value;
    root = "";           //printTo(String) is additive!!
    json.printTo(root);
    //json.printTo(Serial);
    return true;
  }else{
    return false;
  }
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
  }else{
    Serial.println("ERROR File does not exists!");
    return "NIL";
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
