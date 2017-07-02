#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "FFS.h"


//###############################################################################
//  API
//###############################################################################
class API{
public:
  API(SysUtils& sysUtils, FFS& ffs);
  SysUtils& sysUtils;
  FFS& ffs;

  bool call(String topic, String arg);  // "foo/bar","arg1,arg2,arg3"
  bool call(String topicArg);           // "foo/bar arg1,arg2,arg3"
  bool call(TTopic topic);
  TTopic dissectTopic(String topic, String arg);
  void printTopic(TTopic topic);

private:

};
