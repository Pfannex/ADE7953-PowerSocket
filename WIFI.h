#pragma once
  #include <Arduino.h>
  #include <WiFiClient.h> 
  #include "Setup.h"
  #include "FFS.h"
  #include "I2C.h"

//###############################################################################
//  WiFi client 
//###############################################################################
class WIFI{
public:
  WIFI(FFS& ffs, I2C& i2c);
  FFS& ffs;
  I2C& i2c;
  
  bool start();
  
private:
  WiFiClient wifi_client;
};

