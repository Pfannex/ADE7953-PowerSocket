#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include <Wire.h>
#include "modules/oneWire.h"
#include "Adafruit_BMP085.h"
#include "Adafruit_Si7021.h"

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "dynDash"
#define DEVICEVERSION "V1.00"

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  String fillDashboard();
  void on_events(Topic &topic);

private:
  void modifyDashboard();                      //modify dynamicDashboard
  void addText(String name);
  int index = 1;
};
