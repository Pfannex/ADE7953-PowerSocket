#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Core/FFS.h"
#include <Arduino.h>
#include "OneWire.h" //1-Wire
#include "DallasTemperature.h"

//###############################################################################
//  I2C Tools
//###############################################################################

#define OW_Name    "module::OW"
#define OW_Version "0.1.0"

//###############################################################################
//  I2C
//###############################################################################

#define OWDIFF 1
#define OWINT 1

class OW : public Module {

public:
  OW(string name, LOGGING &logging, TopicQueue &topicQueue, int owPin, FFS &ffs);
  int owPin;


  void start();
  void handle();
  String getVersion();

  void scanBus();
  void readDS18B20();
  int owPoll = 5000;
  int count = 0;
  int countOld = 0;
  String sensorenJson;

protected:
  FFS &ffs;

private:
  OneWire oneWire;
  DallasTemperature DS18B20;
  DeviceAddress addr;
  int tPoll = 0;
  bool changed = true;
};
