#pragma once
#include <Arduino.h>
#include <string.h>

//###############################################################################
//  Topic
//###############################################################################
typedef char* string;

class Topic{
public:
  Topic(String topic_asString, String arg_asString);
  ~Topic();

  string* item;
  String topic_asString;
  int countTopics = 0;
  string* arg;
  String arg_asString;
  int countArgs = 0;
  String asString;

  void dissectTopic();
  String deleteTopicItem(int item);


private:
  void printTopic();
};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
