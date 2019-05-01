#pragma once
#include "framework/Core/FFS.h"
#include "Setup.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include "framework/Utils/Logger.h"
//#include "I2C.h"

typedef enum {
  STA_DISCONNECTED,
  STA_CONNECTED,
  STA_UNKNOWN
} sta_state_t;

//ap_state_t  = AP state
typedef enum {
  AP_CLOSED,
  AP_OPEN_WITHOUT_STATION,
  AP_OPEN_WITH_STATION,
  AP_UNKNOWN
} ap_state_t;

#include <functional>
typedef std::function<void(void)> CallbackFunction;
typedef std::function<void(String&)> String_CallbackFunction;
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
  void set_callback(CallbackFunction wifi_init,
                    CallbackFunction wl_connected,
                    CallbackFunction wl_connect_failed,
                    CallbackFunction wl_no_ssid_avail,
                    CallbackFunction ap_closed,
                    CallbackFunction ap_stations_connected,
                    CallbackFunction ap_no_stations_connected,
                    String_CallbackFunction wifi_scan_result
                   );

  void start();
  void handle();

  // API
  String set(Topic &topic);
  String get(Topic &topic);

  sta_state_t getStaState();
  ap_state_t getApState();
  bool hasValidSSID();

  void startSTA(int state);
  void startAP(int state);

  void logIPConfig();

private:
  sta_state_t staState = STA_UNKNOWN;
  ap_state_t  apState  = AP_UNKNOWN;

  // true if status has changed since last update
  bool updateStaStatus(); 
  bool updateApStatus();

  wl_status_t wlState= WL_DISCONNECTED;

  String scanWifi();
  String scanResult();
  void on_scanWifi_complete();
  int scanStatus = -2; //in progress: -1; not been triggered: -2; >0 = found

  CallbackFunction on_wifi_init;
  CallbackFunction on_wl_connected;
  CallbackFunction on_wl_disconnected;
  CallbackFunction on_wl_no_ssid_avail;
  CallbackFunction on_ap_closed;
  CallbackFunction on_ap_stations_connected;
  CallbackFunction on_ap_no_stations_connected;
  String_CallbackFunction on_wifi_scan_result;

  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h
  
};
