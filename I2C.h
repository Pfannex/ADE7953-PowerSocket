#pragma once
  #include <Arduino.h>
  #include "Setup.h"
  #include <SSD1306.h>

//###############################################################################
//  LCD Display 
//###############################################################################
class LCD{
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
class I2C{
public:
  I2C();
  LCD lcd;
  void start();
  
private:

};



