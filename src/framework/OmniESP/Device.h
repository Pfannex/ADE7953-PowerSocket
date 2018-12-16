// SPI.h is part of the Arduino core. It needs to be included here because
// otherwise Arduino sensor libraries fail to compile -
// this is a known issue with the order of header resolution
#include <SPI.h>
#include "framework/Utils/Logger.h"
#include "framework/Core/FFS.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Web/Dashboard.h"


//###############################################################################
//  Device
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  virtual String getType();
  virtual String getVersion();
  String getDashboard();
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

  // Dashboard
  Dashboard dashboard;
  void dashboardChanged(); // fire event

};
