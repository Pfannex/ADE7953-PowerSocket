#pragma once

//BasicTemplate
  #include "FFS.h"
  #include "WIFI.h"
  #include "WEBServer.h"
  #include "MQTT.h"
  #include "oWire.h"
  #include "I2C.h"
  
class BasicTemplate{

public:
  void checkFlash();
  void TimerUpdate();
  
  void startConnections();  
  void startPeriphery();  

  BasicTemplate();
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
