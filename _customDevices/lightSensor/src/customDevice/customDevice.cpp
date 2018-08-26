#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

// http://s6z.de/cms/index.php/arduino/sensoren/15-umgebungslichtsensor-bh1750

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs), sensor(ADDRESS) {}

//...............................................................................
// helper
//...............................................................................

void customDevice::logPollInterval() {
  logging.info("polling sensor every " + String(pollInterval) + "ms");
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  logging.info("starting device " + String(DEVICETYPE));

  Device::start();

  pollInterval = ffs.deviceCFG.readItem("pollInterval").toInt();
  if (pollInterval < 1000)
    pollInterval = POLL_IVL_DEF;
  logPollInterval();

  sensor.begin(RESOLUTION);
  isPresent = sensor.isPresent();
  if (isPresent) {
    logging.info("BH1750 light sensor found at I2C address " +
                 String(ADDRESS, HEX));
  } else {
    logging.error("no BH1750 light sensor found at I2C address " +
                  String(ADDRESS, HEX));
  }

  logging.info("device running");
}

//...............................................................................
// measure - returns illuminance in lux
//...............................................................................

float customDevice::measure() { return sensor.readLightLevel(); }

void customDevice::inform() {
  topicQueue.put("~/event/device/illuminance", measure());
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  if(!isPresent) return;
  unsigned long now = millis();
  if (now - lastPoll >= pollInterval) {
    lastPoll = now;
    // logging.debug("bing...");
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
    └─gpio             (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/pollInterval
    return TOPIC_NO;
  if (topic.itemIs(3, "pollInterval")) {
    pollInterval = topic.getArgAsLong(0);
    logging.info("polling sensor every " + String(pollInterval) + "ms");
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
    illuminance            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/illuminance
    return TOPIC_NO;
  if (topic.itemIs(3, "illuminance")) {
    return String(measure()); // result in lux
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
