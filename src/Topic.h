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
  Topic(char* topics, char* args);
  ~Topic();

  string* item;
  string* arg;

  char* topics;
  char* args;

  int countItems = 0;
  int countArgs = 0;

//to function
  String topic_asString;
  String arg_asString;
  String asString;

  void dissectTopic();
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
