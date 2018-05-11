#pragma once

#include "framework/OmniESP/Controller.h"
#include "framework/OmniESP/Topic.h"
#include <Arduino.h>
#include "framework/Utils/Debug.h"

//#############################################################################
//  API
//#############################################################################

#define MIN_MSECS_BETWEEN_CALLS 200

class API {

public:
  // constructor
  API(Controller &controller);

  // initialize API
  void start();

  // users of the API register their TopicFunctions and LogFunctions to
  // get informed about Topics and log entries
  void registerTopicFunction(TopicFunction TopicFn);
  void registerLogFunction(LogFunction LogFn);

  String call(Topic &topic);
  String call(String topicsArgs);
  String call(string topicsArgs);
  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);

private:
  // the controller
  Controller &controller;

  // last called
  int lastCallTime= 0;

  // device name
  String deviceName;

  // users of the API can register their TopicFunction with registerTopicFunction
  // the API then forwards all Topics to that function
  #define MAXTOPICFUNCTIONS 4
  TopicFunction *topicFunction= new TopicFunction[MAXTOPICFUNCTIONS];
  int topicFunctionCount= 0;

  // users of the API can register their LogFunction with registerLogFunction
  // the API then forwards all log entries to that function
  #define MAXLOGFUNCTIONS 4
  LogFunction *logFunction= new LogFunction[MAXLOGFUNCTIONS];
  int logFunctionCount= 0;

  // the API registers this function with controller to receive all Topics
  void onTopic(const time_t t, Topic &topic);

  // the API registers this function with controller.logging to receive all
  // log entries
  void onLog(const String &channel, const String &message);
};
