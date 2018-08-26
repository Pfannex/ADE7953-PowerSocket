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
//  controller class
//###############################################################################

// time in ms to switch from STA_DISCONNECTED to start the access point
#define STA_TIMEOUT 300000
// time in ms to switch from AP_OPEN_WITHOUT_STATION to try STA reconnect
#define AP_TIMEOUT 300000

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
    void on_wl_connected();
    void on_wl_connect_failed();
    void on_wl_no_ssid_avail();
    void on_ap_stations_connected();
    void on_ap_no_stations_connected();
    void on_wifi_scan_result(String result);
    //LAN
    void on_lanConnected();
    void on_lanDisconnected();
  //internal Events
  void on_netConnected();
  void on_netDisconnected();
  void on_config_mode_on();
  void on_config_mode_off();
  void on_staTimeout();
  void on_apTimeout();
  void on_wifi_state_change();
  //API-Call events
  void on_mqtt_connected();
  void on_mqtt_disconnected();

  bool mqtt_state = 0;

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
  //bool startConnections();
  void viewsUpdate(time_t t, Topic& topic);
  void handleEvent(String& topicsArgs);

  bool startFtp();
  bool startNtp();

  sta_state_t staState = STA_UNKNOWN;
  ap_state_t  apState  = AP_UNKNOWN;
  long staTimeout = STA_TIMEOUT;
  long apTimeout  = AP_TIMEOUT;
  unsigned long long staTimeout_t = 0;
  unsigned long long apTimeout_t  = 0;
  int  staTimeoutActive = false;
  int  apTimeoutActive = false;
  void handleWifiTimout();


  TopicQueue topicQueue;
  // if a new Topic is received this function is called
  TopicFunction topicFunction;
};
