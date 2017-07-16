#pragma once
  #include <Arduino.h>
  #include <WiFiClient.h>
  #include "Setup.h"
  #include "SysUtils.h"

  #include <functional>
  typedef std::function<void(void)> CallbackFunction;

//###############################################################################
//  WiFi client
//###############################################################################
class WIFI{
public:
  WIFI(SysUtils& sysUtils);
  SysUtils& sysUtils;
  //API& api;
  //FFS& ffs;
  //I2C& i2c;

  WiFiClient client;
  void set_callbacks(CallbackFunction wifiConnected,
                     CallbackFunction x);

  bool start();
  bool handle();

private:
  CallbackFunction on_wifiConnected;
  CallbackFunction on_x;


};
