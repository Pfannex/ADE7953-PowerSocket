#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(Controller &controller) : controller(controller) {

  controller.set_callback(std::bind(&API::on_viewUpdate, this));
}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------

//...............................................................................
//  API set callback
//...............................................................................
void API::set_callback(CallbackFunction pubMQTT) {
  on_pubMQTT = pubMQTT;
}

//...............................................................................
//  EVENT ViewUpdate
//...............................................................................
void API::on_viewUpdate(){
  controller.logging.info("callback: API::on_viewUpdate() -> MQTT::on_pubMQTT()");
  if (on_pubMQTT != nullptr) on_pubMQTT();
}

//...............................................................................
//  API start
//...............................................................................
void API::start() {
  info("API started for device with chip ID "+call("~/get/esp/chipId"));
}

//...............................................................................
//  API call distributing
//...............................................................................

String API::call(Topic &topic) {

//TEST TEST
  //if (on_pubMQTT != nullptr) on_pubMQTT();


  // just a pass through
  return controller.call(topic);
}

// convenience function
String API::call(String topicsArgs) {
  Topic tmpTopic(topicsArgs);
  debug("API call "+tmpTopic.asString());
  String result= call(tmpTopic);
  if(result == nullptr) {
      result= String("no result");
  }
  debug("result: "+result);
  return result;
}

// convenience function
String API::call(string topicsArgs) {
  return call(String(topicsArgs));
}


// these are convenience functions
// a more convoluted way would be via a topic ~/log/info

void API::info(const String &msg) {
  controller.logging.info(msg);
}

void API::error(const String &msg) {
  controller.logging.error(msg);
}

void API::debug(const String &msg) {
  controller.logging.debug(msg);
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
