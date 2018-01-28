#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "DeviceSetup.h"
//modules
#include "modules/GPIO.h"

//###############################################################################
//  Device
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  //modules
  //int inputs[2] = {IN01, IN02};
  //GPIOinput GPIOinputs[2];

  GPIOinput GPIOinput_01;
  GPIOinput GPIOinput_02;
  GPIOoutput GPIOoutput_01;
  GPIOoutput GPIOoutput_02;

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  //define device specific functions here #######################################

};
