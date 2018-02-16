#include "LCD.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================
LCD::LCD(int sda, int scl, LOGGING &logging, TopicQueue &topicQueue)
         : sda(sda), scl(scl), logging(logging), topicQueue(topicQueue),
           ssd1306(SSD1306_I2C_adr, sda, scl) {}


//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void LCD::start() {

  logging.info("starting LCD");
  //Wire.begin(sda, scl); // begin(sda, scl)
  ssd1306.init();
  //ssd1306.flipScreenVertically();
  ssd1306.setContrast(255);
  ssd1306.clear();

  println("Hello World!", ArialMT_Plain_24, 0);
  println("Hello World!", ArialMT_Plain_16, 24);
  println("Hello World!", ArialMT_Plain_10, 49);
}

//...............................................................................
// handle
//...............................................................................
void LCD::handle() {
  unsigned long now = millis();

}

//...............................................................................
//  GPIO set
//...............................................................................
String LCD::set(Topic &topic) {
  /*
  ~/set
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("LCD set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "lcd")) {

    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
String LCD::get(Topic &topic) {
  /*
  ~/get
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("LCD get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
void LCD::on_events(Topic &topic){
  //Serial.println(topic.asString());
}

//-------------------------------------------------------------------------------
//  GPIO private
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
