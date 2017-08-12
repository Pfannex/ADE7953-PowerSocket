#pragma once

// Controller
#include "Clock.h"
#include "ESP.h"
#include "FFS.h"
#include "I2C.h"
#include "Logger.h"
#include "Topic.h"
#include "WIFI.h"
#include "oWire.h"

#include <functional>
typedef std::function<void(void)> CallbackFunction;

//###############################################################################
//  BasicTemplate
//###############################################################################
class Controller {
public:
  // constructor
  Controller();

  void set_callback(CallbackFunction pubMQTT);

  void start();
  void handle();
  // Callback Events
  // WiFi
  void on_wifiConnected();
  void on_wifiDisconnected();

  // the subsystems
  Clock clock;
  LOGGING logging;
  FFS ffs;
  WIFI wifi;
  I2C i2c;
  OWIRE oWire;
  ESP_Tools espTools;

  // the API
  String call(Topic &topic);

  // Timer
  void t_1s_Update();
  void t_short_Update();
  void t_long_Update();

private:
  bool startConnections();
  void startPeriphery();
  
  CallbackFunction on_pubMQTT;
};
