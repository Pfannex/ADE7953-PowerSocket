#pragma once
  #include <Arduino.h>
  #include <WiFiClient.h> 

class WIFI{
public:
  WIFI();
  
  bool start();
  
private:
  WiFiClient wifi_client;

};

