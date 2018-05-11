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

class customDevice : public Device{

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
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
  long d01Color = 0;

  // the central modes
  int power = 0;
  int configMode = 0;
  void setConfigMode(int value);
  void setPowerMode(int value);
  void setLedMode();
};
