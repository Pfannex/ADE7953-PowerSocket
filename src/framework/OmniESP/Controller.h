#pragma once

// Controller
#include "framework/Utils/Debug.h"
#include "framework/Core/Clock.h"
#include "framework/Core/ESP.h"
#include "framework/Core/OmniESPUpdater.h"
#include "framework/Core/FFS.h"
#include "framework/Utils/Logger.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Net/WIFI.h"
#include "framework/Utils/SysUtils.h"
#include <ESP8266FtpServer.h>

// THE device
// Dear developer: you need to copy a file set from _customDevices in place
#include "customDevice/customDevice.h"

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
    //LAN
    void on_lanConnected();
    void on_lanDisconnected();
  //internal Events
  void on_netConnected();
  void on_netDisconnected();


  // the subsystems
  Clock clock;
  LOGGING logging;
  FFS ffs;
  WIFI wifi;
  FtpServer ftpSrv;
  ESP_Tools espTools;
  customDevice device;

  // the API
  String call(Topic &topic);

  // Timer
  void t_1s_Update();
  void t_short_Update();
  void t_long_Update();

  // deviceName
  String getDeviceName();

private:
  String deviceName;

  void setConfigDefaults();
  bool setConfigDefault(String item, String defaultValue);
  bool startConnections();
  void viewsUpdate(time_t t, Topic& topic);
  void handleEvent(String& topicsArgs);

  TopicQueue topicQueue;
  // if a new Topic is received this function is called
  TopicFunction topicFunction;
};
