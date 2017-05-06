#pragma once
//FileSystem
  #include <FS.h>                     //this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>
  #include <Arduino.h>

  #include <Arduino.h>

class FFS{
public:
  FFS();
  void TEST();
  size_t getFileSize(String path);
  String readJsonString(String path, bool typ);  //Typ 0 = Array else Object



  
  JsonObject& GetJsonObject(String jsonObject);  
  JsonArray& GetJsonArray(String jsonObject);  
  void parseJsonObject(JsonObject& jsonObject);
  void parseJsonArray(JsonArray& jsonArray);

  String CFG_FilePath         = "/config.json";
  String pub_FilePath         = "/pub.json";
  String sub_FilePath         = "/sub.json";
  String MyFile_FilePath      = "/MyFile.json";
  String ADE7953reg_FilePath  = "/ADE7953.json";
  String testArray_FilePath   = "/testArray.json";
  
  JsonObject& CFG_root        = GetJsonObject("");
  JsonObject& pub_root        = GetJsonObject("");
  JsonObject& sub_root        = GetJsonObject("");
  JsonObject& MyFile_root     = GetJsonObject("");
  JsonObject& ADE7953reg_root = GetJsonObject("");
  

  /*
//json Strings
  String CFG_jsonFileBuffer        = FileReadjsonData(CFG_FilePath);
  String pub_jsonFileBuffer        = FileReadjsonData(pub_FilePath);
  String sub_jsonFileBuffer        = FileReadjsonData(sub_FilePath);
  String MyFile_jsonFileBuffer     = FileReadjsonData(MyFile_FilePath);
  String ADE7953reg_jsonFileBuffer = FileReadjsonData(ADE7953reg_FilePath);
//json Buffer 
  DynamicJsonBuffer CFG_JsonBuffer;
  DynamicJsonBuffer pub_JsonBuffer;
  DynamicJsonBuffer sub_JsonBuffer;
  DynamicJsonBuffer MyFile_JsonBuffer;
  DynamicJsonBuffer ADE7953reg_JsonBuffer;
//json Objects  
  JsonObject& pub_root = pub_JsonBuffer.parseObject(pub_jsonFileBuffer);
  JsonObject& sub_root = sub_JsonBuffer.parseObject(sub_jsonFileBuffer);

  String pubTopic(String item);
  String subTopic(String item);
  void parseJSONarray(JsonArray& jsonArray);
  void parseJSONobject(JsonObject& jsonObject);*/
  
  //String getTopic(String item, JsonObject& root);

  
private:

};

