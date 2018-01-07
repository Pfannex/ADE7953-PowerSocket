#include "FFS.h"

//###############################################################################
//  FFS
//###############################################################################

FFS::FFS(LOGGING &logging)
    : logging(logging),
      // jsonFiles
      cfg(logging, CFG_PATH, TYPE_OBJECT), sub(logging, SUB_PATH, TYPE_OBJECT),
      subGlobal(logging, SUB_GLOBAL_PATH, TYPE_OBJECT), pub(logging, PUB_PATH, TYPE_OBJECT),
      ade7953(logging, ADE7953_PATH, TYPE_OBJECT) {}

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
    // logging.info("formating FS...");
    // SPIFFS.format();
    // logging.info("OK");

    // load rootStrings
    logging.info("loading configuration");
    cfg.loadFile();
    subGlobal.loadFile();
    sub.loadFile();
    pub.loadFile();
    ade7953.loadFile();
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
  } else if (topic.itemIs(3, "sub")) {
    tmpFile = &sub;
  } else if (topic.itemIs(3, "subGlobal")) {
    tmpFile = &subGlobal;
  } else if (topic.itemIs(3, "pub")) {
    tmpFile = &pub;
  } else if (topic.itemIs(3, "ade7953")) {
    tmpFile = &ade7953;
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
  } else if (topic.itemIs(3, "sub")) {
    tmpFile = &sub;
  } else if (topic.itemIs(3, "subGlobal")) {
    tmpFile = &subGlobal;
  } else if (topic.itemIs(3, "pub")) {
    tmpFile = &pub;
  } else if (topic.itemIs(3, "ade7953")) {
    tmpFile = &ade7953;
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
    return false;
  }
}

//...............................................................................
//  read Item from jsonObjectString
//...............................................................................
String FFSjsonFile::readItem(String itemName) {
  DynamicJsonBuffer JsonBuffer;
  JsonObject &rootObject = JsonBuffer.parseObject(root);
  if (rootObject.success()) {
    return rootObject[itemName].as<char*>();
  } else {
    return "NIL";
  }
}

//...............................................................................
//  read Item from jsonArrayString
//...............................................................................
String FFSjsonFile::readItem(int item) {
  DynamicJsonBuffer JsonBuffer;
  JsonArray &rootArray = JsonBuffer.parseArray(root);
  if (rootArray.success()) {
    return rootArray[item].as<char*>();
  } else {
    return "NIL";
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
  String jsonData;

  logging.info("reading " + filePath);
  if (SPIFFS.exists(filePath)) {
    jsonFile = SPIFFS.open(filePath, "r");
    if (jsonFile) {
      logging.debug("file opened");
      size = jsonFile.size();

      DynamicJsonBuffer jsonBuffer;
      JsonVariant json = jsonBuffer.parse(jsonFile);
      if (json.is<JsonArray>()) {
        JsonArray &arr = json.as<JsonArray>();
        json.printTo(jsonData);
      }
      if (json.is<JsonObject>()) {
        JsonObject &obj = json.as<JsonObject>();
        json.printTo(jsonData);
        itemsCount = parseJsonObject(obj);
      }
    } else {
      logging.error("could not open file");
      return "NIL";
    }
    jsonFile.close();
  }
  return jsonData;
}

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
