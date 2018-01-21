#include "framework/Utils/Logger.h"
#include "device/DeviceSetup.h"
#include "framework/Topic.h"

//###############################################################################
//  GPIO
//###############################################################################
//INPUT
// time in ms button needs to be pressed to detect long mode
#define LONGPRESSTIME 5000
// time in ms pin needs to be released againg to detect double click
#define DOUBLECLICKTIME 750
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 50
// time in ms to detect idling
#define IDLETIME 30000

//OUTPUT
// time in ms for blinking
#define BLINKTIME 100


class GPIO {

public:
  GPIO(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  enum outputMode_t { OFF, ON, BLINK, OFT};

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  //GPIO input handling
  int getInputState(int gpioPin);                            // debouncer and state detector
  void handleInput(int gpioPin);        // call handle by GPIO-pin
  int pinState[17];                     // the last pin state
  unsigned long lastDebounceTime[17];   // when the pin state changed last
  int lastPinState[17];                 // state of the last real toggle
  unsigned long pinChangeTime[17];      // time delta measurement
  int pinIdle[17];                      // last time for AUTO-idle
  int pinLongPress[17];                 // longpress detection
  unsigned long pinReleaseTime[17];     // time delta measurement

  //GPIO output handling
  void setOutputMode(int gpioPin, outputMode_t mode, int t); // mode setter
  void handleOutput(int gpioPin);
  outputMode_t currentOutputMode[17];
  int currentOutputOFTtime[17];
  int currentOutputState[17];
};
