#pragma once
  #include <ArduinoJson.h>
  #include <Arduino.h>
  #include "framework/Utils/Logger.h"
  #include "framework/Core/FFS.h"
  #include "Setup.h"
  #include "framework/OmniESP/Topic.h"
  #include "framework/Utils/Debug.h"
  #include "framework/Web/Widget.h"


//###############################################################################
//  Dashboard
//###############################################################################

class Dashboard {
public:
  Dashboard(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void setDashboard(int number);

  String buildDashboard();
  String page_main();

private:
  LOGGING &logging;
  TopicQueue &topicQueue;
  FFS &ffs;

};
