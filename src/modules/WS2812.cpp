#include "WS2812.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================
WS2812::WS2812(int GPIOOutputPin, int LEDsCount,
               LOGGING &logging, TopicQueue &topicQueue)
              :pin(GPIOOutputPin), count(LEDsCount),
               logging(logging), topicQueue(topicQueue) {}


//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void WS2812::start() {
  logging.info("starting WS2812");
  pinMode(pin, OUTPUT);
  ws2812.setBrightness(100);
  ws2812.begin();
  ws2812.show(); // Initialize all pixels to 'off'

}

//...............................................................................
// handle
//...............................................................................
void WS2812::handle() {
  //unsigned long now = millis();

}

//...............................................................................
//  GPIO set
//...............................................................................
String WS2812::set(Topic &topic) {
  /*
  ~/set
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("WS2812 set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  String strPin = topic.getArg(0);

  if (topic.itemIs(3, "ws2812")) {
    if (topic.getArgCount() > 1 and strPin.toInt() == pin){
      WS2812_setColor(topic.getArgAsLong(1));
      return TOPIC_OK;
    }
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
String WS2812::get(Topic &topic) {
  /*
  ~/get
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("WS2812 get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
//  WS2812::on_events
//...............................................................................
void WS2812::on_events(Topic &topic){
  //Serial.println(topic.asString());
}

//...............................................................................
//  WS2812_on
//...............................................................................
void WS2812::WS2812_on(int on, int color) {
  int _color = 0;
  if (on) _color = color;
  for (int i = 0; i<count; i++){
    ws2812.setPixelColor(i, _color);
  }
  ws2812.show();
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------

//...............................................................................
//  set WS2812 color
//...............................................................................
void WS2812::WS2812_setColor(uint32_t R, uint32_t G, uint32_t B) {
  for (int i = 0; i<count; i++){
    ws2812.setPixelColor(i, R, G, B);
  }
  ws2812.show();
}
void WS2812::WS2812_setColor(uint32_t color) {
  for (int i = 0; i<count; i++){
    ws2812.setPixelColor(i, color);
  }
  ws2812.show();
  if (color != 0){
    topicQueue.put("~/event/Device/WS2812 on");
  }else{
    topicQueue.put("~/event/Device/WS2812 off");
  }
}
