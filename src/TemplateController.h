#pragma once

//TemplateController
  #include "SysUtils.h"
  #include "Topic.h"
  #include "API.h"
  #include "FFS.h"
  #include "WIFI.h"
  #include "WEBServer.h"
  #include "MQTT.h"
  #include "oWire.h"
  #include "I2C.h"

//###############################################################################
//  BasicTemplate
//###############################################################################
class TemplateController{
public:
  void start();
  void handle();
//Callback Events
  //WiFi
  void on_wifiConnected();
  void on_x();

//classes
  TemplateController();
  SysUtils sysUtils;
  API api;
  FFS ffs;
  WIFI wifi;
  MQTT mqtt;
  WEBIF webif;
  I2C i2c;
  OWIRE oWire;

//Timer
  void t_1s_Update();
  void t_short_Update();
  void t_long_Update();

private:
  void startConnections();
  void startPeriphery();
};
