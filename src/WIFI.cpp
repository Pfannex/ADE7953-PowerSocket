#include <ESP8266WiFi.h>            //https://github.com/esp8266/Arduino
#include "WIFI.h"

//###############################################################################
//  WiFi
//###############################################################################
WIFI::WIFI(SysUtils& sysUtils, API& api, FFS& ffs, I2C& i2c):
      sysUtils(sysUtils),
      api(api),
      ffs(ffs),
      i2c(i2c){
}

//-------------------------------------------------------------------------------
//  WiFi public
//-------------------------------------------------------------------------------

//...............................................................................
//  WiFi start connection
//...............................................................................
bool WIFI::start(){
  bool WiFiOK = false;

  String ssid = ffs.cfg.readItem("wifiSSID");
  String psk = ffs.cfg.readItem("wifiPSK");

  sysUtils.logging.log("WIFI", "Connecting WiFi to: " + ssid);
  i2c.lcd.println("Connecting WiFi to:", ArialMT_Plain_10, 0);
  i2c.lcd.println(ssid, ArialMT_Plain_16,  10);

  WiFi.mode(WIFI_STA);                     //exit AP-Mode if set once
  WiFi.begin(ssid.c_str(),psk.c_str());
  int i = 0;
  while (WiFi.status() != WL_CONNECTED and i < 31) {
    delay(500);
    i++;
  }

  if (WiFi.status() == WL_CONNECTED){
    WiFiOK = true;
    sysUtils.logging.log("DHCP", WiFi.localIP().toString());
    i2c.lcd.println("DHCP-IP:", ArialMT_Plain_10, 31);
    i2c.lcd.println(WiFi.localIP().toString(), ArialMT_Plain_16, 41);

    if (on_wifiConnected != nullptr) on_wifiConnected();  //callback event
  }else{
    sysUtils.logging.error("Wifi unable to connect");
    //start AP Ã¼ber Button!!! (GodMode?)
  }
  Serial.println("............................................");
  return WiFiOK;
}

//...............................................................................
//  WiFi start connection
//...............................................................................
bool WIFI::handle(){
  if (WiFi.status() == WL_CONNECTED)
    return true;
  else
    return false;
}

//...............................................................................
//  WiFi start connection
//...............................................................................
void WIFI::set_callbacks(CallbackFunction wifiConnected,
                         CallbackFunction x){
  on_wifiConnected = wifiConnected;
  on_x = x;
}

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  xxx
//...............................................................................
