#pragma once

// Controller
#include "Debug.h"
#include "Clock.h"
#include "ESP.h"
#include "FFS.h"
#include "I2C.h"
#include "Logger.h"
#include "Topic.h"
#include "WIFI.h"
#include "oWire.h"
#include "SysUtils.h"
// device specific
#include "Device.h"

//###############################################################################
//  BasicTemplate
//###############################################################################
class Controller {
public:
  // constructor
  Controller();

  // set the callback function for Topics
  void setTopicFunction(TopicFunction topicFn);

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
  Device device;

  // the API
  String call(Topic &topic);

  // Timer
  void t_1s_Update();
  void t_short_Update();
  void t_long_Update();

private:
  String deviceName;

  bool startConnections();
  void viewsUpdate(time_t t, Topic& topic);
  void handleEvent(String& topicsArgs);

  TopicQueue topicQueue;
  // if a new Topic is received this function is called
  TopicFunction topicFunction;
};
