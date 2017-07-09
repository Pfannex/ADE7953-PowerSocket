#pragma once
  #include <Arduino.h>

//###############################################################################
//  1-Wire Bus
//###############################################################################
class Topic{
public:
  Topic();

  String item[10];
  String itemAsString;
  int countTopics;
  String arg[10];
  String argAsString;
  int countArgs;
  String asString;

  void dissectTopic(String strTopic, String strArg);
  String deleteTopicItem(String topic, int item);


private:

};

//###############################################################################
//  API types
//###############################################################################
struct TTopic{
};
