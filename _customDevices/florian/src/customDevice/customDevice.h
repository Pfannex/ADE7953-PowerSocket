#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...
#include <Wire.h>
#include "Adafruit_BMP085.h"
#include "Adafruit_Si7021.h"
#include "modules/MCP23017.h"
#include "modules/WS2812.h"


//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "Device_00"
#define DEVICEVERSION "V0.10"

// maintainer: ...your name...

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  void readBMP180(String name);
  void readSi7021(String name);
  String pressure;

  MCP23017 mcpGPIO;
  void configMCP();
  WS2812 Drawer_01;
  uint8_t R = 0;
  uint8_t G = 0;
  uint8_t B = 0;
  uint32_t color = 0;




  // ...your sensor objects, modules, functions here...
  unsigned long configItem= DEFAULT_VALUE; // exammple
  float measure(); // example
  void inform(); // example
  unsigned long lastPoll= 0;
};
