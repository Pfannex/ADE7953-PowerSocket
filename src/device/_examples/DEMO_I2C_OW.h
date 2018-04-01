#include <ESP8266WiFi.h>
#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "device/Device.h"
#include "device/DeviceSetup.h"
//#include <I2C.h>
#include <Wire.h>

//setup
#include "DEMO_I2C_OW_Setup.h"
//modules
#include "modules/LCD.h"
#include "modules/oneWire.h"
#include "modules/MCP23017.h"
//#include "modules/QRE1113.h"
//#include "modules/WS2812.h"
#include "Adafruit_BMP085.h"
#include "Adafruit_Si7021.h"

//###############################################################################
//  Device
//###############################################################################

class DEMO_I2C_OW : public Device{

public:
  DEMO_I2C_OW(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  //I2C i2c;
  LCD lcd;
  //OW ow;
  //MCP23017 mcpGPIO;
  void configMCP();

  int sensorPollTime;
  unsigned long lastPoll = 0;
  void readBMP180();
  void readSi7021();

};
