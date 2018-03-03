#include "I2C.h"

//===============================================================================
//  I2C
//===============================================================================
I2C::I2C(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl)
        :Module(name, logging, topicQueue),
         sda(sda), scl(scl)
         {}


//-------------------------------------------------------------------------------
//  I2C public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void I2C::start() {
  Module::start();
  logging.info("starting I2C");
  Wire.begin(sda, scl);
  scanBus();
}

//...............................................................................
// handle
//...............................................................................
void I2C::handle() {
  Module::handle();
}

//...............................................................................
// getVersion
//...............................................................................
String I2C::getVersion() {
  return  String(I2C_Name) + " v" + String(I2C_Version);
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
      } else if (address >= 0x20 & address <= 0x27) {
        sprintf(device, "0x%02x MCP23017", address);
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
