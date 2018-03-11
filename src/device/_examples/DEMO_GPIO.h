#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "device/Device.h"
#include "device/DeviceSetup.h"

//setup
#include "DEMO_GPIO_Setup.h"
//modules
#include "modules/GPIO.h"
#include "modules/QRE1113.h"
#include "modules/WS2812.h"

//###############################################################################
//  Device
//###############################################################################

class DEMO_GPIO : public Device{

public:
  DEMO_GPIO(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  GPIOinput button;
  GPIOoutput led;
  GPIOoutput relay;

  QRE1113 qre;
  WS2812 Drawer_01;

  // the central modes
  int power = 0;
  int configMode = 0;
  void setConfigMode(int value);
  void setPowerMode(int value);
  void setLedMode();
};
