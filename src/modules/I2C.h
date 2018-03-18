#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <Arduino.h>

//I²C
#include <Wire.h>
#include <brzo_i2c.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_Si7021.h>
#include "Adafruit_MCP23017.h"

//###############################################################################
//  I2C Tools
//###############################################################################

#define I2C_Name    "module::I2C"
#define I2C_Version "0.1.0"
//###############################################################################
//  I2C
//###############################################################################
#define I2CPOLL 5000
#define I2CDIFF 1
#define I2CINT 1
#define I2C_CLOCK 800

class I2C : public Module {

public:
  I2C(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl);
  int sda;
  int scl;

  void start();
  void handle();
  String getVersion();

  void scanBus();
  uint8_t writeByte(uint8_t i2cAddr, uint8_t address, uint8_t val);
  uint8_t writeBuf(uint8_t i2cAddr, uint8_t address, uint8_t val[], int bytesCount);
  uint8_t readByte(uint8_t i2cAddr, uint8_t address);
  //uint8_t readBuf(uint8_t address);





  //void readBMP180();    //BMP180 Luftdruck/Temperatur
  //void readSi7021();    //SI7021 Luftfeuchtigkeit/Temperatur

  void testBRZO();

private:
  //int tPoll = 0;

};
