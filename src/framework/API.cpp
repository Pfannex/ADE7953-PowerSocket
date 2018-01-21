#include "framework/API.h"

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
  //D("API: begin call(&Topic)");
  // just a pass through
  //Serial.println(topic.asString());
  debug("API call " + topic.asString());
  String result = controller.call(topic);
  if (result == nullptr) {
    result = String("<no result>");
  }
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
  for (int i = 0; i < topicFunctionCount; i++) {
    topicFunction[i](t, topic);
  }
}

void API::onLog(const String &channel, const String &message) {
  //D(String("API onLog "+message).c_str());
  for (int i = 0; i < logFunctionCount; i++) {
    //Di("API onLog", i);
    logFunction[i](channel, message);
  }
  //Dl;
}
