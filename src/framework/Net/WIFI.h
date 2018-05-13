#pragma once
#include "framework/Core/FFS.h"
#include "Setup.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
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

  wl_status_t start();
  void handle();
  void on_connected();
  void on_disconnected();
  String macAddress();

  // API
  String set(Topic &topic);
  String get(Topic &topic);


private:
  // true if status has changed since last update
  bool updateStatus(wl_status_t status);
  String startAP(bool state);

  CallbackFunction on_wifiConnected;
  CallbackFunction on_wifiDisconnected;

  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h
  wl_status_t wl_status= WL_DISCONNECTED;
};
