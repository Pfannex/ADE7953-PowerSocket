#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include <Wire.h>
#include "modules/MCP23017.h"

//###############################################################################
//  Device
//###############################################################################
#define DEVICETYPE "DEMO_MCP23017"
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
  MCP23017 mcpGPIO;
  void configMCP();
  //Test
};
