#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include <Adafruit_NeoPixel.h>

//###############################################################################
//  WS2812
//###############################################################################

#define WS2812_Name    "module::WS2812"
#define WS2812_Version "0.1.0"

//###############################################################################
//  WS2812
//###############################################################################
// number of LEDs
#define WS2812COUNT 4

class WS2812 : public Module{

public:
  WS2812(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOoutputPin,
         int LEDsCount);
  int pin;
  int count;

  void start();
  void handle();
  String getVersion();

  void WS2812_on(int on, int color);

private:
  Adafruit_NeoPixel ws2812 = Adafruit_NeoPixel(count, pin, NEO_GRB + NEO_KHZ800);
  void WS2812_setColor(uint32_t R, uint32_t G, uint32_t B);
  void WS2812_setColor(uint32_t color);
};
