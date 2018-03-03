#include "DEMO_I2C_OW.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  DeviceDEMO_I2C_OW
//-------------------------------------------------------------------------------
DEMO_I2C_OW::DEMO_I2C_OW(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :Device(logging, topicQueue, ffs),
            i2c("i2c", logging, topicQueue, SDA, SCL),
            lcd("SSD1306", logging, topicQueue, SDA, SCL),
            ow("oneWire", logging, topicQueue, OWPIN)
            {}

//...............................................................................
// device start
//...............................................................................
void DEMO_I2C_OW::start() {
  Device::start();
  logging.info("starting device " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  logging.info("starting " + i2c.getVersion()); //only first time a class is started
  i2c.start();
  logging.info("starting " + lcd.getVersion()); //only first time a class is started
  lcd.start();
  logging.info("starting " + ow.getVersion()); //only first time a class is started
  ow.start();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void DEMO_I2C_OW::handle() {
  i2c.handle();
  lcd.handle();
  ow.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String DEMO_I2C_OW::set(Topic &topic) {
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

String DEMO_I2C_OW::get(Topic &topic) {
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
void DEMO_I2C_OW::on_events(Topic &topic) {
  // central business logic

  if (topic.modifyTopic(0) == "event/wifi/connected"){
    lcd.println(ffs.cfg.readItem("wifi_ip"), ArialMT_Plain_16, 0);
  }

/*
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
*/

}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
