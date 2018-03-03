#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO
//###############################################################################

#define GPIO_Name    "module::GPIO"
#define GPIO_Version "0.2.0"

//###############################################################################
//  GPIO input
//###############################################################################
// time in ms button needs to be pressed to detect long mode
#define LONGPRESSTIME 2000
// time in ms pin needs to be released againg to detect double click
#define DOUBLECLICKTIME 300
// first short click bevor double click
#define FIRSTOFDOUBLE 100
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 30  //50
// time in ms to detect idling
#define IDLETIME 3000

class GPIOinput : public Module {

public:
  GPIOinput(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOinputPin);
  int pin;

  void start();
  void handle();
  String getVersion();

private:
  void irq();                       // irq jumpTo funktion
  void irqHandle();                 // handle irq function
  #define irqOFF 4                  // irq detachInterrupt mode
  void irqSetMode(int mode);        // irq mode setter

  int irqDetected = 0;
  unsigned long lastIrqTime = 0;
  unsigned long doubleOutTime = 0;
  int idleState = 0;
  int tIdle = 0;                    // last time for AUTO-idle
  int tstart = 0;
  int tdown  = 0;
  enum event_t {firstOfDouble, click, doubleClick, longClick, none};
  event_t lastEvent = none;
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
  String getVersion();

  void setOutputMode(outputMode_t mode, int t= 0); // mode setter

private:
  outputMode_t currentOutputMode;
  int currentOutputOFTtime;
  int currentOutputBlinkTime;
  int currentOutputState;
  unsigned long lastOftOnTime;

};
