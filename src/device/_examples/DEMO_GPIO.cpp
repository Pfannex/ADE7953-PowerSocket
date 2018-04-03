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
            button("button", logging, topicQueue, PIN_BUTTON),
            led("led", logging, topicQueue, PIN_LED),
            relay("relay", logging, topicQueue, PIN_RELAY),
            qre("qre", logging, topicQueue, PIN_QRE),
            Drawer_01("ws2812", logging, topicQueue, PIN_WS2812, LEDSCOUNT)
            {}

//...............................................................................
// device start
//...............................................................................
void DEMO_GPIO::start() {
  Device::start();
  logging.info("starting device " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  logging.info("starting " + button.getVersion()); //only first time a class is started
  button.start();
  led.start();
  relay.start();
  setLedMode();

  logging.info("starting " + qre.getVersion()); //only first time a class is started
  qre.start();

  logging.info("starting " + Drawer_01.getVersion()); //only first time a class is started
  Drawer_01.start();
  d01Color = ffs.deviceCFG.readItem("WS2812_01_COLOR").toInt();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void DEMO_GPIO::handle() {
  button.handle();
  led.handle();
  relay.handle();
  qre.handle();
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
  if (topic.itemIs(3, "color")) {
    //setPowerMode(topic.getArgAsLong(0));
    d01Color = topic.getArgAsLong(0);
    Drawer_01.WS2812_on(1, d01Color);
    d01Color = ffs.deviceCFG.writeItem("WS2812_01_COLOR", String(d01Color));
    logging.debug(ffs.deviceCFG.readItem("WS2812_01_COLOR"));
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
  // central business logic

//button
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

//QRE1113
  if (qre.isForModule(topic)) {
    if (qre.isItem(topic, "state")) {
      if (topic.argIs(0, "1")) {
        Drawer_01.WS2812_on(1, d01Color);
        logging.debug(String(d01Color));
      }else if (topic.argIs(0, "0")){
        Drawer_01.WS2812_on(0, 0);
      }
    }
  }
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
//  mode setter
//...............................................................................
void DEMO_GPIO::setPowerMode(int value) {
  power = value;
  topicQueue.put("~/event/device/power", power);
  if (power) {
    relay.setOutputMode(ON);
  } else {
    relay.setOutputMode(OFF);
  }
  setLedMode();
}

void DEMO_GPIO::setConfigMode(int value) {
  if (configMode == value)
    return;
  configMode = value;
  topicQueue.put("~/event/device/configMode", configMode);
  setLedMode();
}

void DEMO_GPIO::setLedMode() {
 if (!configMode) {
    if (power)
      led.setOutputMode(ON);
    else
      led.setOutputMode(OFF);
  } else
    led.setOutputMode(BLINK, 250);
}
