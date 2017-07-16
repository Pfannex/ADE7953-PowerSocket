#pragma once

//API
  #include "Setup.h"
  #include "SysUtils.h"
  #include "Topic.h"
  #include "FFS.h"
  #include "WIFI.h"
  //#include "MQTT.h"
  //#include "WEBServer.h"
  //#include "oWire.h"
  //#include "I2C.h"

//###############################################################################
//  BasicTemplate
//###############################################################################
class API{
public:
  API();

  void start();
  void handle();
//Callback Events
  //WiFi
  void on_wifiConnected();
  void on_x();

//classes
  SysUtils sysUtils;
  FFS ffs;
  WIFI wifi;
  //MQTT mqtt;
  //WEBIF webif;
  //I2C i2c;
  //OWIRE oWire;

//Timer
  void t_1s_Update();
  void t_short_Update();
  void t_long_Update();

//API
  String call(Topic& topic);

private:
  void startConnections();
  void startPeriphery();
};
