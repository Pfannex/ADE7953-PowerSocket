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

/*
# ---------------------------------------------------
# DRAWER
# FHEM:
# ---------------------------------------------------
define drawer MQTT_DEVICE
attr drawer IODev MQTT_Broker
attr drawer publishSet_d1color Drawer_01/set/device/drawer1/color
attr drawer publishSet_d2color Drawer_01/set/device/drawer2/color
attr drawer publishSet_color Drawer_01/set/device/drawer3/color
attr drawer publishSet_off Drawer_01/set/device/drawer3/state
attr drawer publishSet_on Drawer_01/set/device/drawer3/state
attr drawer room 02_Küche
attr drawer stateFormat state
attr drawer subscribeReading_drawer1 Drawer_01/event/device/drawer1/state
attr drawer subscribeReading_drawer2 Drawer_01/event/device/drawer2/state
attr drawer subscribeReading_drawer3 Drawer_01/event/device/drawer3/state
attr drawer verbose 5


#attr drawer publishSet_color         Drawer_01/set/device/drawer3/color
#attr drawer publishSet_drawer3       Drawer_01/set/device/drawer3/state
#attr drawer publishSet_fillDashboard Drawer_01/set/device/fillDashboard x

#attr drawer webCmd hue:rgb:rgb ff0000:rgb 98FF23:rgb 0000ff:on:off
#attr drawer widgetOverride rgb:colorpicker,RGB

#attr drawer publishSet_rgb Drawer_01/set/device/drawer3/color

#---------------------------------------------------

define DrawerColor dummy
attr DrawerColor alias Drawer
attr DrawerColor room 02_Küche
attr DrawerColor setList rgb
attr DrawerColor verbose 5
attr DrawerColor webCmd rgb:rgb ff0000:rgb 00ff00:rgb 0000ff:on:off
attr DrawerColor widgetOverride rgb:colorpicker,RGB

define DrawerColor_n notify DrawerColor:rgb.* set drawer d1color $EVTPART1;;set drawer d2color $EVTPART1

#---------------------------------------------------

define Drawer1.3 dummy
attr Drawer1.3 alias Floor
attr Drawer1.3 room 02_Küche
attr Drawer1.3 setList rgb
attr Drawer1.3 verbose 5
attr Drawer1.3 webCmd rgb:rgb ff0000:rgb 00ff00:rgb 0000ff:on:off
attr Drawer1.3 widgetOverride rgb:colorpicker,RGB

define Drawer1.3n_on notify Drawer1.3:on set drawer on 1
define Drawer1.3n_off notify Drawer1.3:off set drawer off 0
define Drawer1.3n_color notify Drawer1.3:rgb.* set drawer color $EVTPART1;;set drawer on 1

define Drawer1.3n_xx notify WZ_INNEN:on set drawer on 1
define Drawer1.3n_yy notify WZ_INNEN:off set drawer off 0


#---------------------------------------------------
define colorInit notify global:INITIALIZED {use Color}
attr colorInit room 02_Küche

*/
