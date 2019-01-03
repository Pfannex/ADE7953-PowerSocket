#include "framework/OmniESP/Device.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
Device::Device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : dashboard(logging), logging(logging), topicQueue(topicQueue), ffs(ffs) {}

//...............................................................................
// Device start
//...............................................................................

// you need to call this base class' start() method from any derived class
void Device::start() {

  logging.info("starting device");
  logging.info("type   : " + getType());
  logging.info("version: " + getVersion());

  // preload dashboard
  // D("preloading dashboard");
  dashboard.load();
  String jsonDocument = dashboard.asJsonDocument();
  logging.info("dashboard=" + jsonDocument);
}
//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void Device::handle() {}
//...............................................................................
//  Device set
//...............................................................................
String Device::set(Topic &topic) {}
//...............................................................................
//  Device get
//...............................................................................
String Device::get(Topic &topic) {}
//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void Device::on_events(Topic &topic) {}

//...............................................................................
//  Device getType, getVersion
//...............................................................................

String Device::getType() { return type; }

String Device::getVersion() { return version; }

String Device::getDashboard() {
  return dashboard.asJsonDocument();
}

String Device::fillDashboard() {}

void Device::dashboardChanged() { // fire event
  topicQueue.put("~/event/device/dashboardChanged");
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
