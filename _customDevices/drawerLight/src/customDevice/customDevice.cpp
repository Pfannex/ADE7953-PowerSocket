#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
      d1_IN("drawer1", logging, topicQueue, DRAWER1_IN),
      d1_OUT("drawer1", logging, topicQueue, DRAWER1_OUT, DRAWER1_LEDCOUNT) {

  type= String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {

  Device::start();
  /*
  logging.info("starting " +
               button.getVersion()); // only first time a class is started
  */
  d1_IN.start();
  d1_OUT.start();
  d1_OUT.color = String(ffs.deviceCFG.readItem("WS2812_01_COLOR")).toInt();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  d1_IN.handle();
  d1_OUT.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─color            (level 3)
    └─power            (level 3)
    └─led              (level 3)
    └─toggle           (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  // if (topic.getItemCount() != 4) // ~/set/device/(power|toggle)
  // return TOPIC_NO;
  if (topic.itemIs(3, "color")) {
    String str = topic.getArg(0);
    d1_OUT.color = (int) strtol( &str[1], NULL, 16);
    ffs.deviceCFG.writeItem("WS2812_01_COLOR", String(d1_OUT.color));
    ffs.deviceCFG.saveFile();
    d1_OUT.WS2812_on(1);
    return TOPIC_OK;

  } else if (topic.itemIs(3, "power")) {
    d1_OUT.WS2812_on(topic.getArgAsLong(0));
    //setPowerMode(power ? 0 : 1);
    return TOPIC_OK;
    
  } else if (topic.itemIs(3, "toggle")) {
    //setPowerMode(power ? 0 : 1);
    return TOPIC_OK;
  } else if (topic.itemIs(3, "led")) {
    if (topic.itemIs(4, "blink")) {
      //setLedMode(topic.getArgAsLong(0));
      return TOPIC_OK;
    }
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
    //topicQueue.put("~/event/device/power", power);
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic){

  // listen to ~/device/drawer1/state
  if (d1_IN.isForModule(topic)) {
    if (d1_IN.isItem(topic, "state"))
      d1_OUT.WS2812_on(topic.getArgAsLong(0));
  }


/*
  // listen to ~/device/led/setmode
  if (led.isForModule(topic)) {
    if (led.isItem(topic, "setmode"))
      setLedMode(topic.getArgAsLong(0));
  }

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
*/
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
//  mode setter
//...............................................................................
