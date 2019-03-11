#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "modules/GPIO.h"
//#include "modules/WS2812.h"
//#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

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
    String fillDashboard();
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  //WS2812  WS_DI;
  //Adafruit_NeoPixel ws = Adafruit_NeoPixel(LEDCOUNT, WS_PIN, NEO_RGB + NEO_KHZ400);

  CRGB leds[LEDCOUNT];
  void setStrip(int color);
  void setStrip(String col);
  void setStrip(int channel, int state);

  int color = 0;
  int count = 0;

  void setChannel(int channel);
  void handleChannels();
  int pinState[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   // the last pin state
  unsigned long lastChangeTime[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};   // last time pin changed
};
