#include <ESP8266WiFi.h>            //https://github.com/esp8266/Arduino  
#include "WIFI.h"

WIFI::WIFI(){
  
}

bool WIFI::start(){
  bool WiFiOK = false;

  Serial.print("Connecting WiFi to: ");  
  Serial.println("Pf@nne-NET");
  WiFi.begin("Pf@nne-NET","Pf@nneNETwlan_ACCESS");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED and i < 31) {
    delay(500);
    Serial.print(".");
  i++;
  }
  Serial.println("");

  if (WiFi.status() == WL_CONNECTED){
    WiFiOK = true; 
    
  }  
  return WiFiOK; 
}

