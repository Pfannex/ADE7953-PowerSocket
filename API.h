#pragma once
  #include <Arduino.h>

//###############################################################################
//  API types
//###############################################################################
 typedef struct TTopic{
    String item[10];
	int countTopics;
	String arg[10];
	int countArgs;
  };
  
//###############################################################################
//  API 
//###############################################################################
class API{
public:
  API();
  bool set(String topic, String arg);  // "foo/bar","arg1,arg2,arg3"
  bool set(String topicArg);           // "foo/bar arg1,arg2,arg3"
  bool set(TTopic topic);              
  //TTopic tmpTopic;
  
private:
  TTopic dissectTopic(String topic, String arg);
  void printTopic(TTopic topic);

};

