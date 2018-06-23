#pragma once
#include "framework/Core/FFS.h"
#include "Setup.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include "framework/Utils/Logger.h"
//#include "I2C.h"

//wl_status_t  = STA StatusEvents in WiFiClient
//sta_state_t  = STA state
typedef enum {
  STA_DISCONNECTED,
  STA_CONNECTED,
  STA_UNKNOWN
} sta_state_t;

//ap_status_t  = AP StatusEvents
typedef enum {
  AP_STATIONS_CONNECTED,
  AP_NO_STATIONS_CONNECTED
} ap_status_t;

//ap_state_t  = AP state
typedef enum {
  AP_CLOSED,
  AP_OPEN_WITHOUT_STATION,
  AP_OPEN_WITH_STATION,
  AP_UNKNOWN
} ap_state_t;

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
  void set_callback(CallbackFunction wl_connected,
                    CallbackFunction wl_connect_failed,
                    CallbackFunction wl_no_ssid_avail,
                    CallbackFunction ap_stations_connected,
                    CallbackFunction ap_no_stations_connected
                   );

  void start();
  void handle();
  void on_connected();
  void on_disconnected();
  String macAddress();

  // API
  String set(Topic &topic);
  String get(Topic &topic);

  sta_state_t staState = STA_DISCONNECTED;
  ap_state_t  apState  = AP_CLOSED;
  String      staMode;
  String      apMode;
  int         apStationsCount = 0;
  int         validSSID = false;

  void startSTA();
  void startAP(int state);

private:
  // true if status has changed since last update
  bool updateStaStatus();
  bool updateApStatus();

  String scanWifi();
  String scanResult();
  int scanStatus = -2; //in progress: -1; not been triggered: -2; >0 = found

  CallbackFunction on_wl_connected;
  CallbackFunction on_wl_connect_failed;
  CallbackFunction on_wl_no_ssid_avail;
  CallbackFunction on_ap_stations_connected;
  CallbackFunction on_ap_no_stations_connected;

  // https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/include/wl_definitions.h
  wl_status_t wl_status     = WL_DISCONNECTED;
  wl_status_t wl_status_old = WL_DISCONNECTED;
  ap_status_t ap_status     = AP_NO_STATIONS_CONNECTED;
  ap_status_t ap_status_old = AP_NO_STATIONS_CONNECTED;
};
