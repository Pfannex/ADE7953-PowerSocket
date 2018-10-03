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
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  void setPage(int number);

  void fillMenue(JsonObject& root);
  String page_main();
  String page_test();
  String page_scanResult();

private:
  LOGGING &logging;
  TopicQueue &topicQueue;
  FFS &ffs;

};
