#pragma once
//FileSystem
  #include <FS.h>                     //this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "Setup.h"
  #include "I2C.h"

//###############################################################################
//  stringFile
//###############################################################################

class FFSstringFile {
public:
    FFSstringFile(String filePath);
    String filePath;
    String data;

    void loadFile();
    String read();
};

//###############################################################################
//  jsonFile
//###############################################################################
class FFSjsonFile{
public:
  FFSjsonFile(String filePath, int type);
  String filePath;
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
  FFS(SysUtils& sysUtils, I2C& i2c);
  SysUtils& sysUtils;
  I2C& i2c;

//Files
  FFSjsonFile cfg;
  FFSjsonFile sub;
  FFSjsonFile subGlobal;
  FFSjsonFile pub;
  FFSjsonFile ade7953;

  FFSjsonFile testArray;
  FFSjsonFile myFile;
//Functions
  void mount();
  String loadString(String filePath);
  void TEST();
//API
  void set(TTopic topic);
  void get(TTopic topic);
private:
//API

};
