#pragma once
#include <Arduino.h>
#include <string.h>

//###############################################################################
//  Topic
//###############################################################################
typedef char* string;

class Topic{
public:
  //Topic(String topic_asString, String arg_asString);
  Topic(String& topicsArgs);
  Topic(char* topics, char* args);
  ~Topic();

  string* item;
  string* arg;
  string topics;
  string args;

  int countItems = 0;
  int countArgs = 0;

//to function
  String topic_asString;
  String arg_asString;
  String asString;

  void dissectTopic(char* topics, char* arg);
  String deleteTopicItem(int item);
  bool itemIs(int index, string topicName);


private:
  void printTopic();
};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
