#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <SSD1306.h>
#include <Wire.h> //I²C

//###############################################################################
//  LCD
//###############################################################################
#define SSD1306_I2C_adr 0x3c

class LCD {

public:
  LCD(int sda, int scl, LOGGING &logging, TopicQueue &topicQueue);
  int sda;
  int scl;
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  SSD1306 ssd1306;
  void println(String txt, const char *fontData, int yPos);

};
