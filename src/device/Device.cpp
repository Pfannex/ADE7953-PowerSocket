#include "device/Device.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
Device::Device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
       :logging(logging), topicQueue(topicQueue), ffs(ffs){}

//...............................................................................
// Device start
//...............................................................................
void Device::start() {
  logging.info("parent starting " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  //logging.info("Device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void Device::handle() {
}

//...............................................................................
//  Device set
//...............................................................................

String Device::set(Topic &topic) {
  /*
  ~/set
  └─Device             (level 2)
    └─power            (level 3)
  */

  logging.debug("Device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/Device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //setPowerMode(topic.getArgAsLong(0));
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  Device get
//...............................................................................

String Device::get(Topic &topic) {
  /*
  ~/get
  └─Device             (level 2)
    └─power            (level 3)
  */

  logging.debug("Device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/Device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //topicQueue.put("~/event/Device/power", power);
    //return String(power);
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void Device::on_events(Topic &topic) {

  //logging.debug("Device handling event " + topic.asString());
/*
  // central business logic
  if (button.isForModule(topic)) {
    // events from button
    //
    // - click
    if (button.isItem(topic, "click")) {
      // -- short
      if (topic.argIs(0, "short")) {
        if (configMode)
          setConfigMode(0);
        else
          setPowerMode(!power);
      }
      // -- long
      if (topic.argIs(0, "long"))
        setConfigMode(!configMode);
    }
    // - idle
    if (button.isItem(topic, "idle"))
      setConfigMode(0);
  }*/
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
