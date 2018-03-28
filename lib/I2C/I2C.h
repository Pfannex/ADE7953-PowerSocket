#pragma once
#include <Arduino.h>
//I²C
#include <brzo_i2c.h>

//###############################################################################
//  I2C
//###############################################################################
#define DEBUG_I2C 0
#define SDA 4
#define SCL 5
#define I2C_CLOCK 400

class I2C{

public:
  I2C();

  int sda = SDA;
  int scl = SCL;
  uint8_t i2cAddr = 0x00;
  int clockSpeed = I2C_CLOCK;

  void setWire(int sda, int scl);
  void setDevice(uint8_t i2cAddr, int clockSpeed);
  void begin();
  void begin(uint8_t i2cAddr);
  void begin(uint8_t i2cAddr, int clockSpeed);
  uint8_t end();
  String scanBus();   //returns a json

  uint8_t write(uint8_t val);
  uint8_t write(uint8_t val, bool repeated_start);
  uint8_t write(uint8_t i2cAddr, int clockSpeed, uint8_t buf[], int bufCount);
  uint8_t write(uint8_t i2cAddr, int clockSpeed,
                uint16_t reg, uint8_t regSize,
                uint32_t val, uint8_t valSize);
  uint8_t write8_8(uint8_t reg, uint8_t val);
  uint8_t write8_16(uint8_t reg, uint16_t val);

  uint8_t write16_8(uint16_t reg, uint8_t val);
  uint8_t write16_16(uint16_t reg, uint16_t val);
  uint8_t write16_32(uint16_t reg, uint32_t val);

  uint8_t* read(uint8_t val[]);
  uint8_t* read(uint8_t val[], bool repeated_start);
  uint8_t* read(uint8_t i2cAddr, int clockSpeed,
                uint32_t reg, uint8_t regSize,
                uint8_t val[], uint8_t valSize);
  uint8_t  read8_8(uint8_t reg);
  uint16_t read8_16(uint8_t reg);
  uint8_t  read16_8(uint16_t reg);
  uint16_t read16_16(uint16_t reg);
  uint32_t read16_32(uint16_t reg);

private:
  void testI2C();
};
