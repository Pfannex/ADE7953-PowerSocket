#pragma once
#include "FFS.h"
#include "Setup.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include "Logger.h"
//#include "I2C.h"

#include <functional>
typedef std::function<void(void)> CallbackFunction;

//###############################################################################
//  WiFi client
//###############################################################################
class WIFI {

public:
  WIFI(LOGGING& logging, FFS& ffs);

  LOGGING& logging;
  FFS& ffs;

  WiFiClient client;
  bool WiFiStatus= false;
  void set_callbacks(CallbackFunction wifiConnected,
                     CallbackFunction wifiDisconnected);

  bool start();
  bool handle();
  String macAddress();

  // API
  String get(Topic &topic);
  

private:
  CallbackFunction on_wifiConnected;
  CallbackFunction on_wifiDisconnected;
};
