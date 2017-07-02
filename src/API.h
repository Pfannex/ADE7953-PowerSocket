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

  bool set(String topic, String arg);  // "foo/bar","arg1,arg2,arg3"
  bool set(String topicArg);           // "foo/bar arg1,arg2,arg3"
  bool set(TTopic topic);
  TTopic dissectTopic(String topic, String arg);
  void printTopic(TTopic topic);

private:

};
