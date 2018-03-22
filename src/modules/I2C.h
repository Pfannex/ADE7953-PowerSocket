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
#define I2C_CLOCK 400

class I2C : public Module {

public:
  I2C(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl);
  int sda;
  int scl;

  void start();
  void handle();
  String getVersion();

  void setBus(uint8_t i2cAddr, int clockSpeed);
  uint8_t _i2cAddr = 0x00;
  int _clockSpeed = 400;
  void scanBus();

  uint8_t write(uint8_t i2cAddr, int clockSpeed, uint8_t buf[], int bufCount);
  uint8_t write(uint8_t i2cAddr, int clockSpeed,
                uint16_t reg, uint8_t regSize,
                uint32_t val, uint8_t valSize);
  uint8_t write(uint8_t reg, uint8_t val);
  uint8_t write_8_16(uint8_t reg, uint16_t val);
  uint8_t write_16_16(uint16_t reg, uint16_t val);
  uint8_t write_16_32(uint16_t reg, uint32_t val);

  uint8_t* read(uint8_t i2cAddr, int clockSpeed,
                uint32_t reg, uint8_t regSize,
                uint8_t val[], uint8_t valSize);


  //void readBMP180();    //BMP180 Luftdruck/Temperatur
  //void readSi7021();    //SI7021 Luftfeuchtigkeit/Temperatur

  void testBRZO();

private:
  //int tPoll = 0;

};
