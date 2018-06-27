#include "framework/OmniESP/API.h"

//###############################################################################
//  API
//###############################################################################
API::API(Controller &controller) : controller(controller) {}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------

//...............................................................................
//  API register callbacks
//...............................................................................
void API::registerTopicFunction(TopicFunction TopicFn) {
  if (topicFunctionCount < MAXTOPICFUNCTIONS) {
    topicFunction[topicFunctionCount++] = TopicFn;
  }
}

void API::registerLogFunction(LogFunction LogFn) {
  if (logFunctionCount < MAXLOGFUNCTIONS) {
    logFunction[logFunctionCount++] = LogFn;
  }
}
//...............................................................................
//  API start
//...............................................................................
void API::start() {
  // get the device name
  deviceName= controller.getDeviceName();
  // register to receive Topics
  controller.setTopicFunction(std::bind(
      &API::onTopic, this, std::placeholders::_1, std::placeholders::_2));
  // register to reveice log entries
  controller.logging.setLogFunction(std::bind(
      &API::onLog, this, std::placeholders::_1, std::placeholders::_2));
  info("API started for device with chip ID " + call("~/get/esp/chipId"));
}

//...............................................................................
//  API call distributing
//...............................................................................

String API::call(Topic &topic) {

  // a too high API call rate crashes the firmware; this is a rate limit
  /*
  int callTime= millis();
  if(callTime-lastCallTime< MIN_MSECS_BETWEEN_CALLS) return String("<rate limit exceeded>");
  lastCallTime= callTime;
  */

  // set and get commands are passed through the controller to
  // the device for handling (device's onEvent handler).
  // In particular, any command coming in from MQTT is mirrored to MQTT
  // via the onTopic handler.
  // The device can trigger zero, one or several events in reaction to a
  // set or get command. The actual return value is returned in the
  // ~/api/lastResult topic. The original command is returned in the
  // ~/api/lastCommand topic.

  //D("******* API call *******");
  //Ds("call topic:", topic.topic_asCStr());
  //Ds("call value:", topic.arg_asCStr());
  String lastCommand= topic.asString();
  debug("API call " +lastCommand);
  String result = controller.call(topic);
  if (result == nullptr) {
    result = String("<no result>");
  }
  // here we inform all listeners view about the last api call and its result
  time_t t= controller.clock.now();
  Topic topicLastCommand("~/api/lastCommand "+lastCommand);
  onTopic(t, topicLastCommand);
  Topic topicLastResult("~/api/lastResult "+result);
  onTopic(t, topicLastResult);
  return result;
}

// convenience function
String API::call(String topicsArgs) { return call(topicsArgs.c_str()); }

// convenience function
String API::call(string topicsArgs) {
  // D("API: call");
  // D(topicsArgs);
  Topic topic(topicsArgs);
  // Dl;
  return call(topic);
}

// convenience function
String API::call(string topics, string args) {
  Topic topic(topics, args);
  return call(topic);
}


// these are convenience functions
// a more convoluted way would be via a topic ~/log/info

void API::info(const String &msg) { controller.logging.info(msg); }

void API::error(const String &msg) { controller.logging.error(msg); }

void API::debug(const String &msg) { controller.logging.debug(msg); }

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------

//...............................................................................
//  broadcast Topics and log entries
//...............................................................................
void API::onTopic(const time_t t, Topic &topic) {
  // we force the device name into the first item
  topic.setItem(0, deviceName.c_str());
  for (int i = 0; i < topicFunctionCount; i++) {
    topicFunction[i](t, topic);
  }
}

void API::onLog(const String &channel, const String &message) {
  // D(String("API onLog "+message).c_str());
  for (int i = 0; i < logFunctionCount; i++) {
    // Di("API onLog", i);
    logFunction[i](channel, message);
  }
  // Dl;
}
