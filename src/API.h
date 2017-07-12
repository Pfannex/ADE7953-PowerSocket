#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "Topic.h"
  #include "FFS.h"


//###############################################################################
//  API
//###############################################################################
class API{
public:
  API(SysUtils& sysUtils, FFS& ffs);
  SysUtils& sysUtils;
  FFS& ffs;

  String call(Topic& topic);  // "foo/bar","arg1,arg2,arg3"
  //String call(String& topicArg);                   // "foo/bar arg1,arg2,arg3"
  //String call(TTopic topic);
  //TTopic dissectTopic(String topic, String arg);
  //String deleteTopicItem(String topic, int item);

private:

};
