#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :Device(logging, topicQueue, ffs),
            button("button", logging, topicQueue, PIN_BUTTON),
            led("led", logging, topicQueue, PIN_LED),
            relay("relay", logging, topicQueue, PIN_RELAY)
          {}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {
  Device::start();
  logging.info("starting device " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  logging.info("starting " + button.getVersion()); //only first time a class is started
  button.start();
  led.start();
  relay.start();
  setLedMode();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  button.handle();
  led.handle();
  relay.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/(power|toggle)
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    setPowerMode(topic.getArgAsLong(0));
    return TOPIC_OK;
  } else if(topic.itemIs(3, "toggle")) {
    setPowerMode(power ? 0 : 1);
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
    └─power            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    topicQueue.put("~/event/device/power", power);
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

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
void customDevice::setPowerMode(int value) {
  power = value;
  topicQueue.put("~/event/device/power", power);
  if (power) {
    relay.setOutputMode(ON);
  } else {
    relay.setOutputMode(OFF);
  }
  setLedMode();
}

void customDevice::setConfigMode(int value) {
  if (configMode == value)
    return;
  configMode = value;
  topicQueue.put("~/event/device/configMode", configMode);

  if (value == 1){
    topicQueue.put("~/set/wifi/ap 1");
  } else {
    topicQueue.put("~/set/wifi/ap 0");
  }

  setLedMode();
}
void customDevice::setLedMode() {
  if (!configMode) {
    if (power)
      led.setOutputMode(ON);
    else
      led.setOutputMode(OFF);
  } else
    led.setOutputMode(BLINK, 250);
}
