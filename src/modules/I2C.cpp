#include "I2C.h"
//#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================
I2C::I2C(int sda, int scl, LOGGING &logging, TopicQueue &topicQueue)
        : sda(sda), scl(scl), logging(logging), topicQueue(topicQueue) {}


//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void I2C::start() {

  logging.info("starting I2C");
  Wire.begin(sda, scl);
  scanBus();

}

//...............................................................................
// handle
//...............................................................................
void I2C::handle() {
  unsigned long now = millis();

}

//...............................................................................
//  GPIO set
//...............................................................................
String I2C::set(Topic &topic) {
  /*
  ~/set
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("GPIO set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "i2c")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
String I2C::get(Topic &topic) {
  /*
  ~/get
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("GPIO get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
void I2C::on_events(Topic &topic){
  //Serial.println(topic.asString());
}

//...............................................................................
//  scan I2C-Bus for devices
//...............................................................................
void I2C::scanBus() {
  byte error, address;
  int nDevices;

  logging.info("scanning I2C bus");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      char device[30];
      if (address == 0x77) {
        sprintf(device, "0x%02x BMP180", address);
      } else if (address == 0x3c) {
        sprintf(device, "0x%02x SSD1306", address);
      } else if (address == 0x40) {
        sprintf(device, "0x%02x SI7021", address);
      } else if (address >= 0x70 & address <= 0x77) {
        sprintf(device, "0x%02x PCA9544", address);
      } else if (address >= 0x60 & address <= 0x67) {
        sprintf(device, "0x%02x MCP4725", address);
      } else if (address >= 0x38) {
        sprintf(device, "0x%02x ADE7953", address);
      } else {
        sprintf(device, "0x%02x unknown", address);
      };
      logging.info(device);
      nDevices++;
    } else if (error == 4) {
      char txt[50];
      sprintf(txt, "unknown error at address 0x%02x", address);
      logging.info(txt);
    }
  }
  if (nDevices == 0)
    logging.info("no I2C devices found");
  else
    logging.info("I2C bus scan done");
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------
