#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

//###############################################################################
//  Topic
//###############################################################################

#define TOPIC_OK String("ok")
#define TOPIC_NO String("no such topic")

typedef const char* string;
class Topic{
public:
  Topic(String& topicsArgs);       // "foo/bar arg1,arg2,arg3"
  Topic(char* topics, char* args); // "foo/bar","arg1,arg2,arg3"
  ~Topic();

  string* item;
  string* arg;
  char* topics;
  char* args;
  int countItems = 0;
  int countArgs = 0;
  String topic_asString();
  String arg_asString();
  String asString();

  void dissectTopic(char* topics, char* arg);
  String modifyTopic(int index);
  bool itemIs(int index, const string topicName);

private:
  bool isValidJson(String root);
  void printTopic();
};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
