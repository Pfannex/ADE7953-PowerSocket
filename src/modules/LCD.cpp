#include "LCD.h"
#include <Arduino.h>

//===============================================================================
//  LCD
//===============================================================================
LCD::LCD(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl)
        :Module(name, logging, topicQueue),
         sda(sda), scl(scl)//,
         //ssd1306(SSD1306_I2C_adr, sda, scl)
         {}


//-------------------------------------------------------------------------------
//  LCD public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void LCD::start() {
  Module::start();

  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_128x64_i2c_init();
  ssd1306_fillScreen( 0x00 );
  ssd1306_clearScreen();

/*
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_clearScreen();
  ssd1306_printFixed(0,  8, "Normal text", STYLE_NORMAL);
  ssd1306_printFixed(0, 16, "Bold text", STYLE_BOLD);
  ssd1306_printFixed(0, 24, "Italic text", STYLE_ITALIC);
  ssd1306_negativeMode();
  ssd1306_printFixed(0, 32, "Inverted bold", STYLE_BOLD);
  ssd1306_positiveMode();
  delay(3000);
  ssd1306_clearScreen();
  ssd1306_printFixedN(0, 0, "N3", STYLE_NORMAL, FONT_SIZE_8X);
  delay(3000);
*/


/*
  logging.info("starting LCD SSD1306");
  Wire.begin(sda, scl);
  ssd1306.init();
  //ssd1306.flipScreenVertically();
  ssd1306.setContrast(255);
  ssd1306.clear();

  ssd1306.println("Hello World!", ArialMT_Plain_24, 0);
  ssd1306.println("Hello World!", ArialMT_Plain_16, 24);
  ssd1306.println("Hello World!", ArialMT_Plain_10, 49);
*/
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
void LCD::println(String txt, int yPos) {





  ssd1306_printFixed(0,  yPos, txt.c_str(), STYLE_NORMAL);




  //ssd1306.setTextAlignment(TEXT_ALIGN_LEFT);
  //ssd1306.setFont(ArialMT_Plain_16);
  //ssd1306.drawString(0, yPos, txt);
  //ssd1306.display();

  //println("Hello World!", ArialMT_Plain_24, 0);
  //println("Hello World!", ArialMT_Plain_16, 24);
  //println("Hello World!", ArialMT_Plain_16, 49);

}
