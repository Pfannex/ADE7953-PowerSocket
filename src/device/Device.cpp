#include "device/Device.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
Device::Device(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : logging(logging), topicQueue(topicQueue), ffs(ffs),
      button("button", logging, topicQueue, PIN_BUTTON),
      led("led", logging, topicQueue, PIN_LED),
      relay("relay", logging, topicQueue, PIN_RELAY) {}

//...............................................................................
// device start
//...............................................................................
void Device::start() {

  logging.info("starting " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  // start modules
  button.start();
  led.start();
  relay.start();
  setLedMode();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void Device::handle() {
  button.handle();
  led.handle();
  relay.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String Device::set(Topic &topic) {
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
    setPowerMode(topic.getArgAsLong(0));
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
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    topicQueue.put("~/event/device/power", power);
    return String(power);
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void Device::on_events(Topic &topic) {

  //logging.debug("device handling event " + topic.asString());

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
  }
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------

//...............................................................................
//  mode setter
//...............................................................................

void Device::setPowerMode(int value) {
  power = value;
  topicQueue.put("~/event/device/power", power);
  if (power) {
    relay.setOutputMode(ON);
  } else {
    relay.setOutputMode(OFF);
  }
  setLedMode();
}

void Device::setConfigMode(int value) {
  if (configMode == value)
    return;
  configMode = value;
  topicQueue.put("~/event/device/configMode", configMode);
  setLedMode();
}

void Device::setLedMode() {
  if (!configMode) {
    if (power)
      led.setOutputMode(ON);
    else
      led.setOutputMode(OFF);
  } else
    led.setOutputMode(BLINK, 250);
}
