#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include <Wire.h>
#include "modules/LCD.h"
#include "modules/oneWire.h"
//#include "modules/MCP23017.h"
//#include "modules/QRE1113.h"
//#include "modules/WS2812.h"
#include "Adafruit_BMP085.h"
#include "Adafruit_Si7021.h"

//###############################################################################
//  Device
//###############################################################################
#define DEVICETYPE "DEMO_I2C_OW"
#define DEVICEVERSION "V1.00"

class customDevice : public Device{

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LCD lcd;
  //OW ow;
  //MCP23017 mcpGPIO;
  //void configMCP();

  int sensorPollTime;
  unsigned long lastPoll = 0;
  void readBMP180(String name);
  void readSi7021(String name);

};
