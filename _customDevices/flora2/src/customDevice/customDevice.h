#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "Adafruit_BMP280.h"
#include "AS_BH1750.h"
#include "Adafruit_VEML6070.h"
#include "Adafruit_MCP23017.h"
#include <Wire.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "flora"
#define DEVICEVERSION "v2"

// maintainer: Boris Neubert

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  Adafruit_BMP280 pressureSensor;
  AS_BH1750 lightSensor;
  Adafruit_VEML6070 uvSensor;
  Adafruit_MCP23017 mcp;

  unsigned long pollInterval= 0;
  unsigned long lastPoll = 0;
  float measureIlluminanceLux();
  float measureTemperatureCelsius();
  float measurePressurehPa();
  uint16_t measureUVLevel();
  float measureUVmuWpercm2();
  void switchRelay(int relay, int state);
  void inform();
  void logPollInterval();
  bool pressureSensorIsPresent= false;
  bool lightSensorIsPresent= false;
  bool uvSensorIsPresent= false;
  bool mcpIsPresent= false;
};
