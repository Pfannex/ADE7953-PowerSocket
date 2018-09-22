#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs), sensor() {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

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

  logging.info("scanning i2c bus");
  logging.info(Wire.i2c.scanBus());

  Device::start();

  isPresent = sensor.begin(ADDRESS);
  if (isPresent) {
    logging.info("BMP pressure sensor found at I2C address " +
                 String(ADDRESS, HEX));

    pollInterval = ffs.deviceCFG.readItem("pollInterval").toInt();
    if (pollInterval < 1000)
      pollInterval = POLL_IVL_DEF;
    logPollInterval();
  } else {
    logging.error("no BMP pressure sensor found at I2C address 0x" +
                  String(ADDRESS, HEX));
  }

  logging.info("device running");
}

//...............................................................................
// measure - returns illuminance in lux
//...............................................................................

float customDevice::measureTemperatureCelsius() {
  return 0.01*round(100.0*sensor.readTemperature());
}

float customDevice::measurePressurehPa() { return 0.01*round(sensor.readPressure()); }

void customDevice::inform() {
  topicQueue.put("~/event/device/temperature", measureTemperatureCelsius());
  topicQueue.put("~/event/device/pressure", measurePressurehPa());
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  if (!isPresent)
    return;
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
  if (topic.itemIs(3, "pressure")) {
    return String(measurePressurehPa()); // result in hPa
  } else if (topic.itemIs(3, "temperature")) {
    return String(measureTemperatureCelsius()); // result in °C
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
