#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Core/FFS.h"
#include <Arduino.h>
#include "OneWire.h" //1-Wire
#include "DallasTemperature.h"

#include <functional>
typedef std::function<void(String&)> String_CallbackFunction;

//###############################################################################
//  I2C Tools
//###############################################################################

#define OW_Name    "module::OW"
#define OW_Version "0.2.0"

//###############################################################################
//  I2C
//###############################################################################

#define OWDIFF 1
#define OWINT 1

typedef struct{
  String strAddr;
  DeviceAddress addr;
  int value;
} DSdevice;

class OW : public Module {

public:
  OW(string name, LOGGING &logging, TopicQueue &topicQueue, int owPin, FFS &ffs);
  int owPin;
  void set_callbacks(String_CallbackFunction sensorChanged,
                     String_CallbackFunction sensorData);

  void start();
  void handle();
  String getVersion();

  void scanBus();
  void requestData();
  void readDS18B20();
  int owPoll = 5000;
  int count = 0;
  int countOld = 0;

protected:
  FFS &ffs;

private:
  String_CallbackFunction on_SensorChanged;
  String_CallbackFunction on_SensorData;

  OneWire oneWire;
  DallasTemperature DS18B20;
  DSdevice dsDevice[5];
  bool requestRunning = false;
  DeviceAddress addr;
  int tPoll = 0;
};
