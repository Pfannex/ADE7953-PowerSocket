#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO input
//###############################################################################
// time in ms button needs to be pressed to detect long mode
#define LONGPRESSTIME 5000
// time in ms pin needs to be released againg to detect double click
#define DOUBLECLICKTIME 750
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 50  //50
// time in ms to detect idling
#define IDLETIME 30000

class GPIOinput : public Module {

public:
  GPIOinput(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOinputPin);
  int pin;

  void start();
  void handle();

private:
  int getInputState();                   // debouncer and state detector
  int pinState = -1;                     // the last pin state
  unsigned long lastDebounceTime = -1;   // when the pin state changed last
  int lastPinState = -1;                 // state of the last real toggle
  unsigned long pinChangeTime = -1;      // time delta measurement
  int pinIdle = -1;                      // last time for AUTO-idle
  int pinLongPress = -1;                 // longpress detection
  unsigned long pinReleaseTime = -1;     // time delta measurement

  void irq();
  int irqDetected = 0;
  unsigned long lastIrqTime = -1;
  int debouncing = 0;

  int firstDOWN    = 0;
  int firstUP      = 0;
  int firstRUNNING = 0;
  int secondDOWN   = 0;
  int secondUP     = 0;
};

//###############################################################################
//  GPIO output
//###############################################################################

//OUTPUT
// time in ms for blinking
#define BLINKTIME 500

enum outputMode_t { OFF, ON, BLINK, OFT};

class GPIOoutput : public Module {

public:
  GPIOoutput(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOoutputPin);
  int pin;

  void start();
  void handle();

  void setOutputMode(outputMode_t mode, int t= 0); // mode setter

private:
  outputMode_t currentOutputMode;
  int currentOutputOFTtime;
  int currentOutputBlinkTime;
  int currentOutputState;
  unsigned long lastOftOnTime;

};
