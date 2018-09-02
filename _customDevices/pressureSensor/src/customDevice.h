#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "Adafruit_BMP280.h"
#include <Wire.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "pressureSensor"
#define DEVICEVERSION "v1"

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
  Adafruit_BMP280 sensor;
  unsigned long pollInterval= POLL_IVL_DEF;
  unsigned long lastPoll = 0;
  float measureTemperatureCelsius();
  float measurePressurehPa();
  void inform();
  void logPollInterval();
  bool isPresent= false;
};
