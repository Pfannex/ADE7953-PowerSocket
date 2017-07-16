#pragma once

//ClassHandler
  #include "Setup.h"
  #include "API.h"
  #include "SysUtils.h"
  #include "Topic.h"
  #include "FFS.h"
  #include "WIFI.h"
  #include "WEBServer.h"
  #include "MQTT.h"
  #include "oWire.h"
  #include "I2C.h"

//###############################################################################
//  BasicTemplate
//###############################################################################
class ClassHandler{
public:
//classes
  API api;
  SysUtils sysUtils;
  FFS ffs;
  WIFI wifi;
  MQTT mqtt;
  WEBIF webif;
  I2C i2c;
  OWIRE oWire;
private:
};
