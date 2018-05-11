#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <SSD1306Brzo.h>
#include <Wire.h> //I²C

//###############################################################################
//  LCD
//###############################################################################

#define LCD_Name    "module::LCD SSD1306"
#define LCD_Version "0.1.0"

//###############################################################################
//  LCD
//###############################################################################
#define SSD1306_I2C_adr 0x3c

class LCD : public Module {

public:
  LCD(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl);
  int sda;
  int scl;
  void start();
  void handle();
  String getVersion();

  void println(String txt, int yPos);

private:
  SSD1306Brzo ssd1306;
};
