#include "Logger.h"
#include "Setup.h"
#include "Topic.h"

//
// Neopixel
//
#ifdef HAS_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#endif

//
// Button
//
#ifdef HAS_BUTTON
// Arduino Pin 12 = = IO12 = Physical Pin 6 = NodeMCU/WeMos Pin D6
#define BUTTON_PIN 12
// time in ms button needs to be pressed to detect long mode
#define LONGPRESSTIME 5000
// time in ms button needs to be released againg to detect double click
#define DOUBLECLICKTIME 750
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 50
// time in ms to detect idling
#define IDLETIME 30000
#endif

//
// LED
//
#ifdef HAS_LED
// Arduino Pin 15 = IO15 = Physical Pin 16 = NodeMCU/WeMos Pin D8
#define LED_PIN 15
typedef enum ledMode_t { OFF, ON, BLINK };
// time in ms for blinking
#define BLINKTIME 100
#endif

//
// Relay
//
#ifdef HAS_RELAY
// Arduino Pin 14 = IO14 = Physical Pin 5 = NodeMCU/WeMos Pin D5
#define RELAY_PIN 14
#endif

//###############################################################################
//  GPIO
//###############################################################################

class GPIO {

public:
  GPIO(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  void setLedMode(ledMode_t ledMode);
  void setRelayMode(int value);
  void setNeoPixelMode(int value);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

// button
#ifdef HAS_BUTTON
  int buttonPinState[17];             // the last pin state
  unsigned long lastDebounceTime[17]; // when the pin state changed last
  int lastButtonState= 0;
  unsigned long buttonChangeTime = 0;
  int getButtonPinState(int buttonPin);
  int buttonIdle= 0;
  int buttonLongPress = 0;
  unsigned long buttonReleaseTime = 0;
#endif

// led
#ifdef HAS_LED
  ledMode_t currentLedMode;
  void Led(int on);
#endif

// neopixel
#ifdef HAS_NEOPIXEL
  Adafruit_NeoPixel WS_LED =
      Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);
  void Neopixel(int on);
#endif

// relay
#ifdef HAS_RELAY
  void Relay(int on);
#endif
};
