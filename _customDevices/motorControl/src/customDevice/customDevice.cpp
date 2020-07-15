#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs) {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start(); // mandatory

  // ... your code here ...
  configItem = ffs.deviceCFG.readItem("configItem").toInt();
  logging.info("configItem is "+String(configItem));
  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

float customDevice::measure() {
  return 0.815;
}

void customDevice::inform() {
  topicQueue.put("~/event/device/sensor1", measure());
  topicQueue.put("~/event/device/sensor2 foobar");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {

  unsigned long now = millis();
  if (now - lastPoll >= 3000) {
    lastPoll = now;
    inform();
  }
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─yourItem         (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/yourItem
    return TOPIC_NO;
  if (topic.itemIs(3, "yourItem")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
  if (topic.itemIs(3, "sensor1")) {
    return String(measure());
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

  // central business logic
}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................

String customDevice::getDashboard() { return dashboard.asJsonDocument(); }



String customDevice::fillDashboard() {
  //topicQueue.put("~/event/device/drawer/index 1");
  //topicQueue.put("~/set/device/drawer/index 1");

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}
