#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "DeviceSetup.h"
//modules
#include "modules/GPIO.h"
#include "modules/QRE1113.h"
#include "modules/WS2812.h"

//###############################################################################
//  Device
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue, FFS& ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  GPIOinput GPIOinput_01;
  GPIOinput GPIOinput_02;
  QRE1113   qre_01;
  GPIOoutput GPIOoutput_01;
  GPIOoutput GPIOoutput_02;
  WS2812     ws2812_01;

private:
  LOGGING &logging;
  TopicQueue &topicQueue;
  FFS& ffs;

  //define device specific functions here #######################################

};
