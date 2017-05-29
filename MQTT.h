#pragma once
  //#include <FS.h>                     //this needs to be first, or it all crashes and burns...
  //#include <ArduinoJson.h>
  #include <Arduino.h>
  #include "Setup.h"
  #include "FFS.h"
  
//###############################################################################
//  MQTT client 
//###############################################################################
class MQTT{
public:
  MQTT(FFS& ffs);
  FFS& ffs;
  
private:
};

