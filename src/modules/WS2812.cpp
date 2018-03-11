#include "WS2812.h"
#include <Arduino.h>

//===============================================================================
//  WS2812
//===============================================================================
WS2812::WS2812(string name, LOGGING &logging, TopicQueue &topicQueue,
               int GPIOoutputPin, int LEDsCount)
          : Module(name, logging, topicQueue), pin(GPIOoutputPin), count(LEDsCount)
          {}

//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void WS2812::start() {
  Module::start();
  logging.info("setting GPIO pin " + String(pin) + " for WS2812 output");
  pinMode(pin, OUTPUT);
  ws2812.setBrightness(100);
  ws2812.begin();
  ws2812.clear();
}

//...............................................................................
// handle
//...............................................................................
void WS2812::handle() {
}
//...............................................................................
// getVersion
//...............................................................................
String WS2812::getVersion() {
  return  String(WS2812_Name) + " v" + String(WS2812_Version);
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
  String eventPrefix= "~/event/device/" + String(name) + "/";

  for (int i = 0; i<count; i++){
    ws2812.setPixelColor(i, color);
  }
  ws2812.show();
  if (color != 0){
    logging.debug("WS2812 " + String(pin) + " on " + String(color));
    topicQueue.put(eventPrefix + "/state 1");
  }else{
    logging.debug("WS2812 " + String(pin) + " off " + String(color));
    topicQueue.put(eventPrefix + "/state 0");
  }
}
