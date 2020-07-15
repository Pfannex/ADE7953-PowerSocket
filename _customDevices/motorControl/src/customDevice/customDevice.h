#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
// ...your includes here...

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "motorControl"
#define DEVICEVERSION "v1"

// maintainer: Dr. Boris Neubert  omega at online dot de

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  String getDashboard();
  String fillDashboard();
  void on_events(Topic &topic);

private:
  // ...your sensor objects, modules, functions here...
  unsigned long configItem= DEFAULT_VALUE; // exammple
  float measure(); // example
  void inform(); // example
  unsigned long lastPoll= 0;
};
