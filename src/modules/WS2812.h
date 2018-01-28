#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <Adafruit_NeoPixel.h>


//###############################################################################
//  GPIO
//###############################################################################

class WS2812 {

public:
  WS2812(int GPIOOutputPin, int LEDsCount, LOGGING &logging, TopicQueue &topicQueue);
  int pin;
  int count;

  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  void WS2812_on(int on, int color);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(count, pin, NEO_GRB + NEO_KHZ800);
  void WS2812_setColor(uint32_t R, uint32_t G, uint32_t B);
  void WS2812_setColor(uint32_t color);
};
