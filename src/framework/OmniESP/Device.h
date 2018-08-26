#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"

//###############################################################################
//  Device
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  virtual String getType();
  virtual String getVersion();
  virtual void start();
  virtual void handle();
  virtual String set(Topic &topic);
  virtual String get(Topic &topic);
  virtual void on_events(Topic &topic);

protected:
  LOGGING &logging;
  TopicQueue &topicQueue;
  FFS &ffs;

  String type= "generic device";
  String version= "v0";
};
