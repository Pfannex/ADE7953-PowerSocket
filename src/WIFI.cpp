#include "WIFI.h"
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

//###############################################################################
//  WiFi
//###############################################################################
WIFI::WIFI(LOGGING &logging, FFS &ffs) : logging(logging), ffs(ffs) {}

//-------------------------------------------------------------------------------
//  WiFi public
//-------------------------------------------------------------------------------

//...............................................................................
//  WiFi start connection
//...............................................................................
bool WIFI::start() {
  WiFiStatus = false;

  String ssid = ffs.cfg.readItem("wifi_ssid");
  String psk = ffs.cfg.readItem("wifi_password");

  logging.info("connecting WiFi to network with SSID " + ssid);
  /*
  i2c.lcd.println("Connecting WiFi to:", ArialMT_Plain_10, 0);
  i2c.lcd.println(ssid, ArialMT_Plain_16,  10);
  */

  WiFi.mode(WIFI_STA); // exit AP-Mode if set once
  WiFi.begin(ssid.c_str(), psk.c_str());
  int i = 0;
  // wait 30 seconds for connection
  while (WiFi.status() != WL_CONNECTED and i <= 30) {
    delay(1000);
    i++;
    if (!(i % 5)) {
      logging.info("still trying to connect...");
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    WiFiStatus = true;
    logging.info("local IP address: " + WiFi.localIP().toString());
    if (on_wifiConnected != nullptr)
      on_wifiConnected(); // callback event
  } else {
    logging.error("WiFi unable to connect");
    // start AP Ã¼ber Button!!! (GodMode?)
  }
  return WiFiStatus;
}

//...............................................................................
//  WiFi handle connection
//...............................................................................
bool WIFI::handle() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!WiFiStatus) {
      WiFiStatus = true;
      if (on_wifiConnected != nullptr)
        on_wifiConnected();
    }
  }

  else {
    if (WiFiStatus) {
      WiFiStatus = false;
      if (on_wifiDisconnected != nullptr)
        on_wifiDisconnected();
    }
  }
  return WiFiStatus;
}

//...............................................................................
//  WiFi set callbacks
//...............................................................................
void WIFI::set_callbacks(CallbackFunction wifiConnected,
                         CallbackFunction wifiDisconnected) {
  on_wifiConnected = wifiConnected;
  on_wifiDisconnected = wifiDisconnected;
}

//...............................................................................
//  get MACAddress
//...............................................................................
String WIFI::macAddress() {
    uint8_t mac[6];
    char maddr[18];
    WiFi.macAddress(mac);
      sprintf(maddr, "%02x:%02x:%02x:%02x:%02x:%02x",
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return String(maddr);
}

//...............................................................................
//  API
//...............................................................................
String WIFI::get(Topic &topic) {
  if (topic.itemIs(4, "macAddress")) {
    return macAddress();
  } else {
    return TOPIC_NO;
  }

}

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  xxx
//...............................................................................
