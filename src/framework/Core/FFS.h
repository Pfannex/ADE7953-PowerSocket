#pragma once
//FileSystem
  #include <FS.h>                     //this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "framework/Utils/Logger.h"
  #include "framework/OmniESP/Topic.h"
  #include "Setup.h"
  #include "framework/Utils/Debug.h"
  //#include "device/DeviceSetup.h"

//###############################################################################
//  stringFile
//###############################################################################

class FFSstringFile {
public:
    FFSstringFile(LOGGING& logging, String filePath);
    String filePath;
    LOGGING& logging;
    String data;

    void loadFile();
    String read();
};

//###############################################################################
//  jsonFile
//###############################################################################
class FFSjsonFile{
public:
  FFSjsonFile(LOGGING& logging, String filePath, int type);
  String filePath;
  LOGGING& logging;
  size_t size;                               //filled in readJsonString()
  int itemsCount;
  int type;                                  //0=Object; 1=Array
  String root;

  void loadFile();
  String readItem(String itemName);
  String readItem(int item);
  bool writeItem(String itemName, String value);
  bool saveFile();

private:
  String readJsonString();
  int parseJsonObject(JsonObject& jsonObject);
  void parseJsonArray(JsonArray& jsonArray);
};

//###############################################################################
//  FFS
//###############################################################################

class FFS {
public:
  FFS(LOGGING& logging);
  LOGGING& logging;

//Files
  FFSjsonFile vers;
  FFSjsonFile cfg;
  FFSjsonFile deviceCFG;
  FFSjsonFile webCFG;

//Functions
  void mount();
  String loadString(String filePath);
  bool isValidJson(String root);
//API
  String set(Topic& topic);
  String get(Topic& topic);

  uint32_t totalMemory();

private:

};
