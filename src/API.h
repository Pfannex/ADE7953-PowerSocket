#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "Topic.h"
  #include "FFS.h"
  //#include "MQTT.h"

//#############################################################################
//  API
//#############################################################################
class API{
public:
  API(SysUtils& sysUtils, FFS& ffs);
  SysUtils& sysUtils;
  FFS& ffs;
  //MQTT mqtt;

  String call(Topic& topic);
private:
};
