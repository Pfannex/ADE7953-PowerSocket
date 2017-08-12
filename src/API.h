#pragma once
#include "Controller.h"
#include "Topic.h"
#include <Arduino.h>

#include <functional>
typedef std::function<void(void)> CallbackFunction;
typedef std::function<void(Topic&)> Topic_CallbackFunction;

//#############################################################################
//  API
//#############################################################################
class API {

public:
  // constructor
  API(Controller& controller);
  Controller& controller;

  void set_callbackMQTT(Topic_CallbackFunction pubMQTT);
  void set_callbackWEBIF(Topic_CallbackFunction pubWEBIF);

  void start();
  // Callback Events
  // controller
  void on_viewsUpdate(Topic &topic);

  String call(Topic &topic);
  String call(String topicsArgs);
  String call(string topicsArgs);
  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);

private:
  Topic_CallbackFunction on_pubMQTT;
  Topic_CallbackFunction on_pubWEBIF;
};
