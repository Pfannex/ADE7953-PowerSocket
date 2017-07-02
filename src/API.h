#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "FFS.h"


//###############################################################################
//  API
//###############################################################################
class API{
public:
  API(FFS& ffs);
  FFS& ffs;

  bool call(String topic, String arg);  // "foo/bar","arg1,arg2,arg3"
  bool call(String topicArg);           // "foo/bar arg1,arg2,arg3"
  bool call(TTopic topic);
  String set(String topic, String arg);
  String set(TTopic topic);
  String get(String topic, String arg); // get can has arguments, why not?
  String get(TTopic topic);
  TTopic dissectTopic(String topic, String arg);
  String stripTopic(TTopic topic);
  void printTopic(TTopic topic);

private:

};
