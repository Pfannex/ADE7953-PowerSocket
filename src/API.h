#pragma once
#include "Controller.h"
#include "Topic.h"
#include <Arduino.h>

//#############################################################################
//  API
//#############################################################################
class API {

public:
  // constructor
  API(Controller& controller);
  Controller& controller;

  void set_callback(CallbackFunction pubMQTT);

  void start();
  String call(Topic &topic);
  String call(String topicsArgs);
  String call(string topicsArgs);
  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);

private:
  CallbackFunction on_pubMQTT;
};
