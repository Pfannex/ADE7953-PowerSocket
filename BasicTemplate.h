#pragma once

//BasicTemplate
  #include "Helpers.h"
  #include "FFS.h"
  #include "WIFI.h"
  #include "WEBServer.h"
  #include "MQTT.h"
  #include "oWire.h"
  #include "I2C.h"
  
//###############################################################################
//  BasicTemplate
//###############################################################################
class BasicTemplate{
public:
  
  void startConnections();  
  void startPeriphery(); 
  void handle();
  
//Callback Events
  //WiFi   
  void on_wifiConnected();
  void on_x();

//classes
  BasicTemplate();
  Helpers h;
  FFS ffs;
  WIFI wifi;
  MQTT mqtt;
  WEBIF webif;  
  I2C i2c;
  OWIRE oWire; 
 
private:
  void timerUpdate();
  long timerLastUpdate;
  long timerUpdateTime = 5000;
};
