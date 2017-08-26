#include "Setup.h"
#include "Logger.h"

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
#define BTIME 5000
#define DEBOUNCETIME 50
typedef struct {
  int S;           // mode S 0 or 1
  int L;           // mode L 0 or 1
  unsigned long t; // time of last change
  int state;       // button down 0 or 1
  int idle;        // state lasts longer than >= BTIME
} buttonMode_t;
#endif

//
// LED
//
#ifdef HAS_LED
// Arduino Pin 15 = IO15 = Physical Pin 16 = NodeMCU/WeMos Pin D8
#define LED_PIN 15
typedef enum ledMode_t { OFF, ON, BLINK };
#define BLINK_T 5 // in multiples of TIMER_T
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
  GPIO(LOGGING &logging);
  void start();
  void handle();

private:
  LOGGING &logging;


// button
#ifdef HAS_BUTTON
  int buttonPinState[17];             // the last pin state
  unsigned long lastDebounceTime[17]; // when the pin state changed last
  buttonMode_t currentButtonMode;
  unsigned long buttonChangeTime = 0;
  int getButtonPinState(int buttonPin);
  void printButtonMode(String msg, buttonMode_t mode);
  void setButtonMode(buttonMode_t mode);
  void onSetButtonMode(buttonMode_t oldMode, buttonMode_t newMode);
#endif

// led
#ifdef HAS_LED
  ledMode_t currentLedMode;
  void Led(int on);
#endif

// relay
#ifdef HAS_RELAY
  void Relay(int on);
  //void switchRelay(int state) 
#endif

// neopixel
#ifdef HAS_NEOPIXEL
  Adafruit_NeoPixel WS_LED =
      Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);
  void Neopixel(int on);
#endif
};
