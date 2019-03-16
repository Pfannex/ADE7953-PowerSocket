#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
      ow("oneWire", logging, topicQueue, OWPIN, ffs){

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start();
  sensorPollTime = ffs.deviceCFG.readItem("sensorPollTime").toInt();
  logging.info("1wire sensors polling time: " + String(sensorPollTime));
  logging.info("starting device " + String(DEVICETYPE) + String(DEVICEVERSION));

  //ow.start();
  ow.owPoll = sensorPollTime;

  logging.info("device running");

  logging.info("scanning I2C-Bus for devices");
  logging.info(Wire.i2c.scanBus());

}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  ow.handle();

  unsigned long now = millis();
  if (now - lastPoll >= sensorPollTime) {
    lastPoll = now;
    readBMP180("sensor1");
    readSi7021("sensor2");
  }
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─yourItem         (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/yourItem
    return TOPIC_NO;
  if (topic.itemIs(3, "yourItem")) {
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
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
  if (topic.itemIs(3, "sensor1")) {
    return "";
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {
  if (topic.modifyTopic(0) == "event/device/sensorsChanged") {
    handleSensors(topic.arg_asString());
  }
}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................
String customDevice::fillDashboard() {
  //topicQueue.put("~/event/device/drawer/index 1");
  //topicQueue.put("~/set/device/drawer/index 1");

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//...............................................................................
// read BMP180
//...............................................................................
void customDevice::handleSensors(String sen) {
  Serial.println("SensorsChanged");
  Serial.println(sen);
}

//...............................................................................
// read BMP180
//...............................................................................
void customDevice::readBMP180(String name) {
   Adafruit_BMP085 bmp;
   String eventPrefix= "~/event/device/" + name + "/";

   bmp.begin();
   String value = "temperature " + String(bmp.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "pressure " + String(bmp.readPressure()/100);
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}

//...............................................................................
// read SI7021
//...............................................................................
void customDevice::readSi7021(String name) {
   Adafruit_Si7021 si;
   String eventPrefix= "~/event/device/" + name + "/";

   si.begin();
   String value = "temperature " + String(si.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "humidity " + String(si.readHumidity());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}
