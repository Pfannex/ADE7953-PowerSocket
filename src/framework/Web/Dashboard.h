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
  void buildDashboard();
  String getDashboard();

  void sayHello();

private:
  LOGGING &logging;
  TopicQueue &topicQueue;
  FFS &ffs;

};
