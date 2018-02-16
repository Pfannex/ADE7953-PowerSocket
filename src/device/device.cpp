#include "device/device.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
device::device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
       :logging(logging), topicQueue(topicQueue), ffs(ffs){}

//...............................................................................
// device start
//...............................................................................
void device::start() {
  logging.info("parent starting " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  //logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void device::handle() {
}

//...............................................................................
//  Device set
//...............................................................................

String device::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/power
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

String device::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //topicQueue.put("~/event/device/power", power);
    //return String(power);
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void device::on_events(Topic &topic) {

  //logging.debug("device handling event " + topic.asString());
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
