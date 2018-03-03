#include "LCD.h"
#include <Arduino.h>

//===============================================================================
//  LCD
//===============================================================================
LCD::LCD(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl)
        :Module(name, logging, topicQueue),
         sda(sda), scl(scl),
         ssd1306(SSD1306_I2C_adr, sda, scl)
         {}


//-------------------------------------------------------------------------------
//  LCD public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void LCD::start() {
  Module::start();

  logging.info("starting LCD SSD1306");
  ssd1306.init();
  //ssd1306.flipScreenVertically();
  ssd1306.setContrast(255);
  ssd1306.clear();

  //println("Hello World!", ArialMT_Plain_24, 0);
  //println("Hello World!", ArialMT_Plain_16, 24);
  //println("Hello World!", ArialMT_Plain_10, 49);
}

//...............................................................................
// handle
//...............................................................................
void LCD::handle() {
  Module::handle();
}

//...............................................................................
// getVersion
//...............................................................................
String LCD::getVersion() {
  return  String(LCD_Name) + " v" + String(LCD_Version);
}
//-------------------------------------------------------------------------------
//  LCD private
//-------------------------------------------------------------------------------
//...............................................................................
//  printline
//...............................................................................
void LCD::println(String txt, const char *fontData, int yPos) {
  ssd1306.setTextAlignment(TEXT_ALIGN_LEFT);
  ssd1306.setFont(fontData);
  ssd1306.drawString(0, yPos, txt);
  ssd1306.display();

  //println("Hello World!", ArialMT_Plain_24, 0);
  //println("Hello World!", ArialMT_Plain_16, 24);
  //println("Hello World!", ArialMT_Plain_16, 49);

}
