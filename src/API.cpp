#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(Controller &controller) : controller(controller) {

  // callback Events
  // controller
  controller.set_callback(std::bind(&API::on_viewsUpdate, this,
                                    std::placeholders::_1));
}

//-------------------------------------------------------------------------------
//  API public
//-------------------------------------------------------------------------------

//...............................................................................
//  API set callback
//...............................................................................
void API::set_callbackMQTT(Topic_CallbackFunction pubMQTT) {
  on_pubMQTT = pubMQTT;
}
void API::set_callbackWEBIF(Topic_CallbackFunction pubWEBIF) {
  on_pubWEBIF = pubWEBIF;
}
void API::set_callbackWEBSocket(Topic_CallbackFunction pubWEBSocket) {
  on_pubWEBSocket = pubWEBSocket;
}


//...............................................................................
//  API start
//...............................................................................
void API::start() {
  info("API started for device with chip ID "+call("~/get/esp/chipId"));
}

//...............................................................................
//  EVENT ViewUpdate
//...............................................................................
void API::on_viewsUpdate(Topic &topic){
  //controller.logging.debug("-> API::on_viewsUpdate()");
  //controller.logging.debug(topic.asString());

  if (on_pubMQTT != nullptr) on_pubMQTT(topic);
  if (on_pubWEBIF != nullptr) on_pubWEBIF(topic);
  if (on_pubWEBSocket != nullptr) on_pubWEBSocket(topic);
}

//...............................................................................
//  API call distributing
//...............................................................................

String API::call(Topic &topic) {
  //D("API: begin call(&Topic)");
  // just a pass through
  debug("API call "+topic.asString());
  String result= controller.call(topic);
  if(result == nullptr) {
      result= String("<no result>");
  }
  return result;
}

// convenience function
String API::call(String topicsArgs) {
    return call(topicsArgs.c_str());
}

// convenience function
String API::call(string topicsArgs) {
  //D("API: call");
  //D(topicsArgs);
  Topic topic(topicsArgs);
  //Dl;
  return call(topic);
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
