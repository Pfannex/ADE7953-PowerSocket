#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/OmniESP/Device.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include "modules/GPIO.h"

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE      "SimpleSwitch"
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

  GPIOinput button;
  GPIOoutput led;
  GPIOoutput relay;

  // the central modes
  int power = 0;
  int configMode = 0;
  void setConfigMode(int value);
  void setPowerMode(int value);
  void setLedMode(int value);
};
