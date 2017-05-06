#pragma once
  #include <FS.h>                     //this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>
  #include <Arduino.h>

class MQTT{
public:
  MQTT();
  
  String pub_FilePath = "/pub.json";
  String sub_FilePath = "/sub.json";
  String pub_jsonFileBuffer = FileReadjsonData(pub_FilePath);
  String sub_jsonFileBuffer = FileReadjsonData(sub_FilePath);
  DynamicJsonBuffer pub_JsonBuffer;
  JsonObject& pub_root = pub_JsonBuffer.parseObject(pub_jsonFileBuffer);
  DynamicJsonBuffer sub_JsonBuffer;
  JsonObject& sub_root = sub_JsonBuffer.parseObject(sub_jsonFileBuffer);

  String pubTopic(String item);
  String subTopic(String item);
  void parseJSONarray(JsonArray& jsonArray);
  void parseJSONobject(JsonObject& jsonObject);
  size_t getFileSize(String path);

private:
  String FileReadjsonData(String path);
  String getTopic(String item, JsonObject& root);
};

