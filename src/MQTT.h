#pragma once
  #include <Arduino.h>
  #include "API.h"
  #include "SysUtils.h"
  #include "Setup.h"
  #include "FFS.h"
  #include "WIFI.h"
  #include <PubSubClient.h>

//###############################################################################
//  MQTT client
//###############################################################################
class MQTT{
public:
  MQTT(SysUtils& sysUtils, API& api, FFS& ffs, I2C& i2c, WIFI& wifi);
  SysUtils& sysUtils;
  API& api;
  FFS& ffs;
  I2C& i2c;
  WIFI& wifi;

  bool start();
  bool handle();
  void setCallback(MQTT_CALLBACK_SIGNATURE);
  void on_incommingSubcribe(char* topic, byte* payload, unsigned int length);


private:
  PubSubClient client;
};
