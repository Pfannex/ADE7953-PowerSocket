#include <ESP8266WiFi.h>            //https://github.com/esp8266/Arduino  
#include "WIFI.h"

//###############################################################################
//  WiFi client 
//###############################################################################
WIFI::WIFI(FFS& ffs, I2C& i2c):
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

  Serial.print("Connecting WiFi to: ");  
  i2c.lcd.println("Connecting WiFi to:", ArialMT_Plain_10, 0);  
  i2c.lcd.println(ffs.cfg.readItem("wifiSSID"), ArialMT_Plain_16,  10);  
  
  Serial.println(ffs.cfg.readItem("wifiSSID"));
  WiFi.begin(ffs.cfg.readItem("wifiSSID").c_str(),ffs.cfg.readItem("wifiPSK").c_str());
  int i = 0;
  while (WiFi.status() != WL_CONNECTED and i < 31) {
    delay(500);
    Serial.print(">");
  i++;
  }

  if (WiFi.status() == WL_CONNECTED){
    WiFiOK = true; 
    Serial.println("connected");
    Serial.print("DHCP-IP: "); Serial.println(WiFi.localIP().toString());   
    i2c.lcd.println("DHCP-IP:", ArialMT_Plain_10, 31);  
    i2c.lcd.println(WiFi.localIP().toString(), ArialMT_Plain_16, 41);        
  }else{
    Serial.println("NOT connected!");   
  }
  Serial.println("............................................");
  return WiFiOK; 
}

//-------------------------------------------------------------------------------
//  WiFi private
//-------------------------------------------------------------------------------

//...............................................................................
//  xxx
//...............................................................................

