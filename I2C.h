#pragma once
  #include <Arduino.h>
  #include "Setup.h"
  #include <SSD1306.h>

//device addresses
  #define SSD1306_I2C_adr 0x3c

//===============================================================================
//  LCD Display 
//===============================================================================
class LCD{
public:
  LCD();
  void init();
  void println(String txt, int line);
  void clear();

private:  
  SSD1306 ssd1306;
}; 
 
//===============================================================================
//  I2C 
//===============================================================================
class I2C{
public:
  I2C();
  LCD lcd;
  void start();
  
private:

};



