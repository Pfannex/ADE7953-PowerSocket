#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "modules/GPIO.h"
#include "modules/QRE1113.h"
#include "modules/WS2812.h"

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE      "drawerLight"
#define DEVICEVERSION   "v1"

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  QRE1113 d1_IN;
  WS2812  d1_OUT;
  QRE1113 d2_IN;
  WS2812  d2_OUT;
};
