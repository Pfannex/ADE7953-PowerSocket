#pragma once
  #include <Arduino.h>
  //#include "API.h"
  #include "SysUtils.h"
  #include "Setup.h"
  //#include "FFS.h"
  #include "WIFI.h"
  #include <PubSubClient.h>

//###############################################################################
//  MQTT client
//###############################################################################
class MQTT{
public:
  MQTT(SysUtils& sysUtils, WIFI& wifi);
  SysUtils& sysUtils;
  //FFS& ffs;
  //I2C& i2c;
  WIFI& wifi;
  

  bool start();
  bool handle();
  void setCallback(MQTT_CALLBACK_SIGNATURE);
  void on_incommingSubcribe(char* topic, byte* payload, unsigned int length);
  void pub(String topic, String value);
//API
  String set(Topic& topic);
  String get(Topic& topic);

private:
  PubSubClient client;
};
