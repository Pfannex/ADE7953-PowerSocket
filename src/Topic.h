#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

//###############################################################################
//  Topic
//###############################################################################
typedef char* string;
class Topic{
public:
  Topic(String& topicsArgs);       // "foo/bar arg1,arg2,arg3"
  Topic(char* topics, char* args); // "foo/bar","arg1,arg2,arg3"
  ~Topic();

  string* item;
  string* arg;
  string topics;
  string args;
  int countItems = 0;
  int countArgs = 0;
  String topic_asString();
  String arg_asString();
  String asString();

  void dissectTopic(char* topics, char* arg);
  String modifyTopic(int index);
  bool itemIs(int index, string topicName);

private:
  bool isValidJson(String root);
  void printTopic();
};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
