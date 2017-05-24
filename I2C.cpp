#include "I2C.h"

//===============================================================================
//  I2C 
//===============================================================================
I2C::I2C(){
}

void I2C::start(){
  lcd.init();
}

//===============================================================================
//  LCD Display 
//===============================================================================
LCD::LCD():
    ssd1306(SSD1306_I2C_adr, I2C_SDA, I2C_SCL){
}

void LCD::init(){
  ssd1306.init();
  ssd1306.flipScreenVertically();
  ssd1306.setContrast(255);
}

void LCD::println(String txt, int line){
  ssd1306.setTextAlignment(TEXT_ALIGN_LEFT);
  ssd1306.setFont(ArialMT_Plain_16);
  line = line * 16;
  ssd1306.drawString(0, line, txt);
  ssd1306.display();
}
void LCD::clear(){
  ssd1306.clear();
}

