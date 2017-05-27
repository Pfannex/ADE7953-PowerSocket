#pragma once
//FileSystem
  #include <FS.h>                     //this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "Setup.h"
  #include "I2C.h"
 
//===============================================================================
//  jsonFile
//===============================================================================
class FFSjsonFile{
public:  
  FFSjsonFile(String filePath, int type);
  String filePath;
  size_t size;                               //filled in readJsonString()
  int type;                                  //0=Object; 1=Array
  String root;

  void loadFile();

  bool read(String itemName);
  bool write(String itemName, String value);
 
  String readItem(String itemName);
  String readItem(int item);
  bool writeItem(String itemName, String value);

private: 
  String readJsonString();
  //DynamicJsonBuffer JsonBuffer;
  //JsonObject& jsonRootObject;
  //JsonArray& jsonRootArray;

  void parseJsonObject(JsonObject& jsonObject);
  void parseJsonArray(JsonArray& jsonArray);
};
 

//===============================================================================
//  FFS 
//===============================================================================
class FFS{
public:
  FFS(I2C& i2c);
  I2C& i2c;
  
  FFSjsonFile cfg;
  FFSjsonFile testArray;
  FFSjsonFile myFile;

  void mount();
  
private:
};

