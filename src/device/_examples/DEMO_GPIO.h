#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/Topic.h"
#include "device/deviceSetup.h"
#include "device/Device.h"
//modules
#include "modules/GPIO.h"

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

  GPIOinput button;


private:
};
