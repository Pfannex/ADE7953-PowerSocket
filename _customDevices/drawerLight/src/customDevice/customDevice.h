#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "modules/GPIO.h"
#include <FastLED.h>

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE      "drawerLight"
#define DEVICEVERSION   "V1"

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
    String fillDashboard();
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  CRGB leds[LEDCOUNT];
  void setChannel(int channel);
  void setStrip(int color);
  void setStrip(String col);
  void setStrip(int channel, int state);
  void handleChannels();

  int index = 1;
  String name = "";
  String color = "";
  String usecase = "";

  int pinState[16] = {};   // the last pin state
  unsigned long lastChangeTime[16] = {};   // last time pin changed
};
