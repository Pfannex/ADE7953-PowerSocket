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

  void setWire(int _sda, int _scl);
  void setDevice(uint8_t _i2cAddr, int _clockSpeed);
  void start();
  void start(uint8_t _i2cAddr);
  void start(uint8_t _i2cAddr, int _clockSpeed);
  uint8_t stop();
  String scanBus();   //returns a json
  bool isPresent(uint8_t _i2cAddr);

//write
  //write various size of register/buffer, incl. start/stop
  uint8_t write(uint8_t i2cAddr, int clockSpeed, uint8_t buf[], int bufCount);
  uint8_t write(uint8_t i2cAddr, int clockSpeed,
                uint16_t reg, uint8_t regSize,
                uint32_t val, uint8_t valSize);
  //write various size of buffer, incl. start/stop
  uint8_t write8(uint8_t reg, uint8_t val[], int valCount);
  uint8_t write16(uint16_t reg, uint8_t val[], int valCount);
  uint8_t write32(uint32_t reg, uint8_t val[], int valCount);
  //write to a xbyte register xbyte value, incl. start/stop
  uint8_t write8_8(uint8_t reg, uint8_t val);
  uint8_t write8_16(uint8_t reg, uint16_t val);
  uint8_t write16_8(uint16_t reg, uint8_t val);
  uint8_t write16_16(uint16_t reg, uint16_t val);
  uint8_t write16_32(uint16_t reg, uint32_t val);
  //write 1 Byte excl. start/stop
  uint8_t writeRAW(uint8_t val);
  uint8_t writeRAW(uint8_t val, bool repeated_start);

//read
  //read various size of register/buffer, incl. start/stop
  void read(uint8_t i2cAddr, int clockSpeed,
            uint32_t reg, uint8_t regSize,
            uint8_t *val, uint8_t valSize);
  //read various size of buffer, incl. start/stop
  void read8(uint8_t reg, uint8_t *val, int valCount);
  void read16(uint16_t reg, uint8_t *val, int valCount);
  void read32(uint32_t reg, uint8_t *val, int valCount);
  //read from a xbyte register xbyte value, incl. start/stop
  uint8_t  read8_8(uint8_t reg);
  uint16_t read8_16(uint8_t reg);
  uint8_t  read16_8(uint16_t reg);
  uint16_t read16_16(uint16_t reg);
  uint32_t read16_32(uint16_t reg);
  //read 1 Byte excl. start/stop
  void readRAW(uint8_t *buf);
  void readRAW(uint8_t *buf, bool repeated_start);


  void testI2C();

private:
  bool addresses[128];
  String deviceAdd(String& devices, String device);
};
