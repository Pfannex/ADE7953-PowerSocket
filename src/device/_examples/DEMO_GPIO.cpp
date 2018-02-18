#include "DEMO_GPIO.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
DEMO_GPIO::DEMO_GPIO(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :Device(logging, topicQueue, ffs),
            button("Walter", logging, topicQueue, PIN_BUTTON)
            {}

//...............................................................................
// device start
//...............................................................................
void DEMO_GPIO::start() {
  Device::start();
  logging.info("child starting " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  button.start();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void DEMO_GPIO::handle() {
  button.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String DEMO_GPIO::set(Topic &topic) {
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

String DEMO_GPIO::get(Topic &topic) {
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
void DEMO_GPIO::on_events(Topic &topic) {

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
