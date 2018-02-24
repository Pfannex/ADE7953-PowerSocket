#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "device/Device.h"
#include "device/DeviceSetup.h"

//setup
#include "simpleSwitch_Setup.h"
//modules
#include "modules/GPIO.h"

//###############################################################################
//  Device
//###############################################################################

class simpleSwitch : public Device{

public:
  simpleSwitch(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
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
  void setLedMode();
};
