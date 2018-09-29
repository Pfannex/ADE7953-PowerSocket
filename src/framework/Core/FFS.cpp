#include "framework/Core/FFS.h"

// the documentation for Arduino JSON is here
// https://arduinojson.org/api/

//###############################################################################
//  FFS
//###############################################################################

FFS::FFS(LOGGING &logging)
    : logging(logging),
      // jsonFiles
      vers(logging, VERS_PATH, TYPE_OBJECT),
      cfg(logging, CFG_PATH, TYPE_OBJECT),
      deviceCFG(logging, DEVICECFG_PATH, TYPE_OBJECT),
      webCFG(logging, DASHBOARD_PATH, TYPE_OBJECT){}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------

//...............................................................................
//  Mount FFS
//...............................................................................
void FFS::mount() {

  logging.info("mounting flash file system");
  if (!SPIFFS.begin()) {
    logging.error("failed to mount flash file system");
  } else {
    logging.info("flash file system mounted");
     //logging.info("formatting FS...");
     //SPIFFS.format();
     //logging.info("OK");

    FSInfo fs_info;
    SPIFFS.info(fs_info);
    char txt[128];
    sprintf(txt, "%luKiB/%luKiB used, block size: %luB, page size: %luB",
      fs_info.usedBytes/1024, fs_info.totalBytes/1024,
      fs_info.blockSize, fs_info.pageSize);
    logging.debug(String(txt));


    // load rootStrings
    logging.info("loading version");
    vers.loadFile();
    logging.debug("version loaded");
    logging.info("loading configuration");
    cfg.loadFile();
    deviceCFG.loadFile();
    webCFG.loadFile();
    logging.debug("configuration loaded");
  }
}

//...............................................................................
//  load string from file system
//...............................................................................

String FFS::loadString(String filePath) {

  FFSstringFile FS(logging, filePath);
  FS.loadFile();
  return FS.read();
}

//...............................................................................
//  check for valid JSON-String
//...............................................................................
bool FFS::isValidJson(String root) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(root);
  return json.success();
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
      ├─loadFile
      ├─saveFile
      ├─root          [jsonString]    RW
      └─item
          └─itemName  [value]         RW
*/
String FFS::set(Topic &topic) {
  String str = "NIL";

  FFSjsonFile *tmpFile = NULL;
  if (topic.itemIs(3, "cfg")) {
    tmpFile = &cfg;
  } else if (topic.itemIs(3, "deviceCFG")) {
    tmpFile = &deviceCFG;
  } else if (topic.itemIs(3, "webCFG")) {
    tmpFile = &webCFG;
  } else if (topic.itemIs(3, "version")) {
    tmpFile = &vers;
  }

  if (tmpFile != NULL) {
    // LoadFile
    if (topic.itemIs(4, "loadFile")) {
      tmpFile->loadFile();
      return "OK";
      // SaveFile
    } else if (topic.itemIs(4, "saveFile")) {
      tmpFile->saveFile();
      return "OK";
      // write rootString
    } else if (topic.itemIs(4, "root")) {
      if (topic.getArgCount() > 0) {
        if (isValidJson(topic.getArg(0))) {
          tmpFile->root = topic.getArg(0);
          return "OK";
        } else {
          logging.error("no valid JSON string");
          return "no valid JSON string";
        }
      } else {
        logging.error("argument not found");
        return "argument not found!";
      }

      // writeItem
    } else if (topic.itemIs(4, "item")) {
      if (topic.getItemCount() > 5) {
        if(topic.getArgCount() > 0) {
          tmpFile->writeItem(topic.getItem(5), topic.getArg(0));
        } else {
          tmpFile->writeItem(topic.getItem(5), "");
        }
        return tmpFile->readItem(topic.getItem(5));
      } else {
        logging.error("no configuration item given");
        return "no configuration item given";
      }
    }
    // ERROR--------------------------------------------------
  } else {
    logging.error("no matching file found");
    return "no matching file found";
  }
}

//...............................................................................
//  API GET
//...............................................................................
/*
~/get/
ffs
  └─fileObject
     ├─filePath     R
     ├─size         R
     ├─itemsCount   R
     ├─type         R
     ├─root         RW
     └─item
         └─itemName RW
*/
String FFS::get(Topic &topic) {
  FFSjsonFile *tmpFile = NULL;
  if (topic.itemIs(3, "cfg")) {
    tmpFile = &cfg;
  } else if (topic.itemIs(3, "deviceCFG")) {
    tmpFile = &deviceCFG;
  } else if (topic.itemIs(3, "webCFG")) {
    tmpFile = &webCFG;
  } else if (topic.itemIs(3, "version")) {
    tmpFile = &vers;
  }

  if (tmpFile != NULL) {
    // filePath
    if (topic.itemIs(4, "filePath")) {
      return tmpFile->filePath;
      // FileSize
    } else if (topic.itemIs(4, "size")) {
      return String(tmpFile->size);
      // ItemsCount
    } else if (topic.itemIs(4, "itemsCount")) {
      return String(tmpFile->itemsCount);
      // Json Object Type
    } else if (topic.itemIs(4, "type")) {
      return String(tmpFile->type);
      // write rootString
    } else if (topic.itemIs(4, "root")) {
      return tmpFile->root;
      // readItem
    } else if (topic.itemIs(4, "item")) {
      return tmpFile->readItem(topic.getItem(5));
    } else {
      return TOPIC_NO;
    }
    // ERROR
  } else {
    logging.error("no matching file found");
    return "NIL";
  }
}

//-------------------------------------------------------------------------------
//  FFS private
//-------------------------------------------------------------------------------

//###############################################################################
//  FFSstringFile
//###############################################################################

FFSstringFile::FFSstringFile(LOGGING& logging,
String filePath) : logging(logging), filePath(filePath) {}

//-------------------------------------------------------------------------------
//  FFSstringFile public
//-------------------------------------------------------------------------------

//...............................................................................
//  load string from FFS file
//...............................................................................

void FFSstringFile::loadFile() {

  File stringFile;

  logging.info("reading file " + filePath);
  if (SPIFFS.exists(filePath)) {
    stringFile = SPIFFS.open(filePath, "r");
    if (stringFile) {
      logging.debug("file opened");
      data = stringFile.readString();
      stringFile.close();
    } else {
      logging.error("could not open file");
      data = "";
    }
  } else {
    logging.error("no such file");
  }
}

//...............................................................................
//  get string
//...............................................................................

String FFSstringFile::read() { return data; }

//###############################################################################
//  FFSjsonFile
//###############################################################################

FFSjsonFile::FFSjsonFile(LOGGING& logging, String filePath, int type)
    : logging(logging), filePath(filePath), type(type) {}

//-------------------------------------------------------------------------------
//  FFSjsonFile public
//-------------------------------------------------------------------------------

//...............................................................................
//  add or replace Array @key in root
//...............................................................................
int FFSjsonFile::set_toRoot(String key, JsonArray& newArray){
int result = 0;  //1=OK, 0=key not found,

/*
json structure
[{                            //root is array with object widgets
  "widget_1":[{               //code of widget_1 is nestedArray in rootArray/objectWidget
    "type":"group",
    "name":"group1",
    "data":[{
      "type":"controllgroup",
      "name":"myName",
      "data":[1,2]
    }]
  }],
  "widget_2":[{
    "type":"group",
    "name":"group1",
    "data":[{
      "type":"controllgroup",
      "name":"myName",
      "data":[1,2]
    }]
  }]
}]
*/

  //String root = readJsonString();
  DynamicJsonBuffer jsonBuffer;
  JsonArray &rootArray = jsonBuffer.parseArray(root);
  JsonObject &rootObject = rootArray[0];

  Serial.println("loaded rootObject");
  rootObject.prettyPrintTo(Serial);
  Serial.println("");

  //Serial.println("array to insert");
  //newArray.prettyPrintTo(Serial);
  //Serial.println("");


  rootObject.set(key, newArray);
  //Serial.println("rootObject after");
  //rootObject.prettyPrintTo(Serial);
  //Serial.println("");
  result++;

  rootArray.set(0, rootObject);
  root = "";                     //printTo is additive
  rootArray.printTo(root);

  //Serial.println("rootArray after");
  //rootArray.prettyPrintTo(Serial);
  //Serial.println("");
  saveFile();

  return result;
}

//...............................................................................
//  load root string from FFS-File (for external use)
//...............................................................................
void FFSjsonFile::loadFile() { root = readJsonString(); }

//...............................................................................
//  save root string to FFS-File (for external use)
//...............................................................................
bool FFSjsonFile::saveFile() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(root);
  if (json.success()) {
    File jsonFile = SPIFFS.open(filePath, "w");
    if (!jsonFile) {
      logging.info("failed to open config file for writing");
      return false;
    }
    json.printTo(jsonFile);
    jsonFile.close();
    return true;
  } else {
    logging.debug("failed to parse json.root!");
    return false;
  }
}

//...............................................................................
//  read Item from jsonObjectString
//...............................................................................
String FFSjsonFile::readItem(String itemName) {
  if(!itemName) {
    logging.error("software error: calling FFSjsonFile::readItem() without argument");
    return "";
  }
  //D(itemName.c_str());
  DynamicJsonBuffer JsonBuffer;
  JsonObject &rootObject = JsonBuffer.parseObject(root);
  if (rootObject.success()) {
    //Dl;
    JsonVariant value= rootObject[itemName];
    if(value.success()) {
      return value.as<char*>();
    } else {
      logging.error("no item "+itemName+" in file "+filePath);
      return "";
    }
  } else {
    logging.error("FFSjsonFile::readItem(String): could not access root object for item "+itemName+" in file "+filePath);
    return "";
  }
}

//...............................................................................
//  read Item from jsonArrayString
//...............................................................................
String FFSjsonFile::readItem(int item) {
  DynamicJsonBuffer JsonBuffer;
  JsonArray &rootArray = JsonBuffer.parseArray(root);
  if (rootArray.success()) {
    if((item>= rootArray.size()) || (item< 0)) {
      logging.error("software error: argument of FFSjsonFile::readItem() out of bounds");
      return "";
    }
    return rootArray[item].as<char*>();
  } else {
    logging.error("FFSjsonFile::readItem(int): could not access root object in file "+filePath);
    return "";
  }
}

//...............................................................................
//  writeItem to jsonObjectString
//...............................................................................
bool FFSjsonFile::writeItem(String itemName, String value) {
  // logging.info(itemName + ":" + value);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(root);
  if (json.success()) {
    // json.printTo(Serial);
    json[itemName] = value;
    root = ""; // printTo(String) is additive!!
    json.printTo(root);
    // json.printTo(Serial);
    return true;
  } else {
    return false;
  }
}

//-------------------------------------------------------------------------------
//  FFSjsonFile private
//-------------------------------------------------------------------------------

//...............................................................................
//  read json-root-string from File
//...............................................................................
String FFSjsonFile::readJsonString() {

  File jsonFile;
  DynamicJsonBuffer jsonBuffer;
  JsonVariant json;
  String jsonData;

  logging.info("reading " + filePath);
  size= 0;
  if (SPIFFS.exists(filePath)) {
    jsonFile = SPIFFS.open(filePath, "r");
    if (jsonFile) {
      logging.debug("file "+filePath+" opened");
      size = jsonFile.size();
      json = jsonBuffer.parse(jsonFile);
      jsonFile.close();
    } else {
      logging.error("could not open file "+filePath+", returning empty list");
      json = jsonBuffer.parse("{}");
    }
  } else {
    logging.error("file "+filePath+" does not exist, returning empty list");
    json = jsonBuffer.parse("{}");
  }
  // parse file content or empty list
  if (json.is<JsonArray>()) {
    JsonArray &arr = json.as<JsonArray>();
    json.printTo(jsonData);
  }
  if (json.is<JsonObject>()) {
    JsonObject &obj = json.as<JsonObject>();
    json.printTo(jsonData);
    itemsCount = parseJsonObject(obj);
  }
  //D(jsonData.c_str());
  return jsonData;
}

//...............................................................................
//  parse json recursively
//...............................................................................
void FFSjsonFile::parseJson(JsonVariant root) {
  //WARNING do not use big jsons -> memory overflow
  //logging.info("parseJson");

  if (root.is<JsonArray>()){
    //logging.info("root isArray");
    JsonArray& array = root;
    for (auto &element : array) {
      JsonVariant variant = element;
      if (element.is<JsonArray &>()){
        //logging.info("isArray");
        parseJson(variant);
      } else if (element.is<JsonObject>()){
        //logging.info("isObject");
        parseJson(variant);
      } else {
        //logging.info("DATA in Array");
        logging.info("   " + String(element.as<char*>()));
      }
    }
  }else if (root.is<JsonObject>()){
    //logging.info("root isObject");
    JsonObject& object = root;
    //JsonArray& array = root;
    for (auto &element : object){
      JsonVariant variant = &element;
      if (variant.is<JsonArray>()){
        //logging.info("isArray");
        parseJson(variant);
      }else if (variant.is<JsonObject>()) {
        //logging.info("isObject");
        parseJson(variant);
      }else{
        String strKey = element.key;
        String strValue = element.value;

        DynamicJsonBuffer jsonBuffer;
        JsonVariant nested = jsonBuffer.parse(strValue);
        if (nested.success() and (nested.is<JsonArray &>() or nested.is<JsonObject>())){
          //logging.info("nestedArray in Object");
          logging.info("   " + strKey);
          parseJson(nested);

        }else{
          //logging.info("DATA in Object");
          logging.info("   " + strKey + " | " + strValue);
        }
      }
    }
  }
}

/*
[{
		"Array0": [1, 2, 3],
		"Object1": {
			"Item11": "Value1",
			"Item12": "Value2",
      "Array": [1,2,{"A1":"B1"}]
		},
		"Object2": {
			"Item21": "Value1",
			"Item22": "Value2"
		}
	},
	{
		"Array1": [{
			"11_Key": "11_Val",
			"12_Key": "12_Key"
		  },1,2,3
    ],
		"Array2": [{
			"21_Key": "21_Val",
			"22_Key": "22_Val"
		}]
}]
*/

/*
{
  "obj1.key": "obj1.val",
  "array1_inObj.key": ["array1.element1", "array1.element2"],
  "array2_inObj.key": ["array2.element1", "array2.element2",
                      {"obj2_inArray.key": "obj2_inArray.val"}]
}
*/

//...............................................................................
//  parse jsonObject recursively
//...............................................................................
int FFSjsonFile::parseJsonObject(JsonObject &jsonObject) {
  int count = 0;
  // logging.info("parseJsonObject");
  for (auto &element : jsonObject) {
    String strKey = element.key;
    String strValue = element.value;
    // logging.info(strKey);logging.info("      ");logging.info(strValue);
    count++;
  }
  return count;
}

//...............................................................................
//  parse jsonArray recursively
//...............................................................................
void FFSjsonFile::parseJsonArray(JsonArray &jsonArray) {
  for (auto &element : jsonArray) {
    if (element.is<JsonArray &>()) {
      logging.info("-->>"); // logging.info(i);
      JsonArray &nestedArray = element;
      parseJsonArray(nestedArray);
    } else {
      logging.info(element.as<char*>()); // logging.info(" | ");logging.info(i);
    }
  }
}
