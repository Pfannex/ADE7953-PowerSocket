#pragma once
#include "Logger.h"
#include "Setup.h"
#include <Arduino.h>
#include <SSD1306.h>
#include <Wire.h> //IÂ²C

//###############################################################################
//  LCD Display
//###############################################################################
class LCD {
public:
  LCD();
  void init();
  void println(String txt, const char *fontData, int yPos);
  void clear();

private:
  SSD1306 ssd1306;
};

//###############################################################################
//  I2C
//###############################################################################
class I2C {
public:
  I2C(LOGGING &logging);
  LOGGING &logging;

  LCD lcd;
  void start();
  void scanBus();

private:
};
