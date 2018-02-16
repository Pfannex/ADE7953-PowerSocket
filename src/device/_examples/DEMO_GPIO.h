#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "device/deviceSetup.h"
#include "device/device.h"
//modules
#include "modules/moduleTemplate.h"
#include "modules/toConvert/MCP23017.h"

//###############################################################################
//  Device
//###############################################################################

class DEMO_GPIO : public device{

public:
  DEMO_GPIO(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();

  GPIOinputX button;
  MCP23017 mcp;


private:
};
