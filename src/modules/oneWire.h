#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
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

#define OWPOLL 5000
#define OWDIFF 1
#define OWINT 1

class OW : public Module {

public:
  OW(string name, LOGGING &logging, TopicQueue &topicQueue, int owPin);
  int owPin;

  void start();
  void handle();
  String getVersion();

  void scanBus();
  void readDS18B20();
  int count = 0;

private:
  OneWire oneWire;
  DallasTemperature DS18B20;
  DeviceAddress DS18B20device;
  int tPoll = 0;
};
