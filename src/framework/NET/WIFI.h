#pragma once
#include "framework/Core/FFS.h"
#include "Setup.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include "framework/Utils/Logger.h"
//#include "I2C.h"

#include <functional>
typedef std::function<void(void)> CallbackFunction;
typedef std::function<void(Topic&)> Topic_CallbackFunction;

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
  void set_callback(CallbackFunction wifiConnected,
                    CallbackFunction wifiDisconnected);

  bool start();
  void handle();
  void on_connected();
  void on_disconnected();
  String macAddress();

  // API
  String get(Topic &topic);


private:
  CallbackFunction on_wifiConnected;
  CallbackFunction on_wifiDisconnected;
};
