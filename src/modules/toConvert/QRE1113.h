#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"

//###############################################################################
//  QRE1113 input
//###############################################################################
// time in ms button needs to be pressed to detect long mode
#define LONGPRESSTIME 5000
// time in ms pin needs to be released againg to detect double click
#define DOUBLECLICKTIME 750
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 50
// time in ms to detect idling
#define IDLETIME 30000

class QRE1113 {

public:
  QRE1113(int GPIOinputPin, LOGGING &logging, TopicQueue &topicQueue);
  int pin;

  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  int readQRE();                         // debouncer and state detector
  int pinState = 0;                      // the last pin state
  unsigned long lastDebounceTime = -1;   // when the pin state changed last
  int lastPinState = -1;                 // state of the last real toggle
  unsigned long pinChangeTime = -1;      // time delta measurement
  int pinIdle = -1;                      // last time for AUTO-idle
  int pinLongPress = -1;                 // longpress detection
  unsigned long pinReleaseTime = -1;     // time delta measurement
};
