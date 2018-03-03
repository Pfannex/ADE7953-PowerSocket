#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <Arduino.h>
#include <Wire.h> //I²C
#include <Adafruit_BMP085_BrzoI2C.h>

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

class I2C : public Module {

public:
  I2C(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl);
  int sda;
  int scl;

  void start();
  void handle();
  String getVersion();

  void scanBus();
  void readBMP180();    //BMP180 Luftdruck/Temperatur

private:
  int tPoll = 0;

};
