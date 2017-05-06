#pragma once

//ESP8266_Basic
  #include "FFS.h"
  #include "WIFI.h"
  #include "WEBServer.h"
  #include "MQTT.h"
  #include "oWire.h"
  #include "I2C.h"
  
class ESP8266_Basic{

public:
  void checkFlash();
  void TimerUpdate();
  
  void startConnections();  

  ESP8266_Basic();
  FFS ffs;
  WIFI wifi;
  MQTT mqtt;
  WEBIF webServer;  
  I2C i2c;
  OWIRE oWire; 
 
private:

  long timerLastUpdate;
  long timerUpdateTime = 5000;   
};
