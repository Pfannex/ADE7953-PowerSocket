#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "DeviceSetup.h"
//modules
#include "GPIO.h"

//###############################################################################
//  Device test
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  GPIO gpio;

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  //define device specific functions here #######################################
  // the central modes
  int power = 0;
  int configMode = 0;
  void setConfigMode(int value);
  void setPowerMode(int value);
  void setLedMode();

};
