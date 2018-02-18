#include "modules/GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIOinput
//===============================================================================
GPIOinput::GPIOinput(string name, LOGGING &logging, TopicQueue &topicQueue,
          int GPIOinputPin)
          : Module(name, logging, topicQueue), pin(GPIOinputPin)
          {
}

//-------------------------------------------------------------------------------
//  GPIOinput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOinput::start() {

  Module::start();
  logging.info("setting GPIO pin " + String(pin) + " for input");
  pinMode(pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin), std::bind(&GPIOinput::irq, this), RISING);
}

//...............................................................................
// handle
//...............................................................................
void GPIOinput::handle() {

  Module::handle();

  while (irqDetected){
    detachInterrupt(pin);
    //lastIrqTime = now;
    irqDetected = 0;
    logging.debug("IRQ");
    attachInterrupt(digitalPinToInterrupt(pin), std::bind(&GPIOinput::irq, this), RISING);
  }

  unsigned long now = millis();
  int pinState = getInputState();
  if (pinState < 0)
    return; // still bouncing



  unsigned long t = now;
  unsigned long tl = t - pinChangeTime;
  int idling = (tl >= IDLETIME);
  int longtime = (tl >= LONGPRESSTIME);

  String eventPrefix= "~/event/device/" + String(name) + "/";
  // note the difference:
  // the short event is created when the button is released after a short time
  // the long event is created when the button is pressed for a long time
  if (pinState == lastPinState) {
    // pin unchanged
    if (pinState) {
      // pin is pressed
      if (!pinLongPress && longtime) {
        pinLongPress = 1;
        topicQueue.put(eventPrefix + "click long");
      }
    } else {
      // pin is not pressed
      if (!pinIdle && idling) {
        pinIdle = 1;
        topicQueue.put(eventPrefix + "idle 1");
      }
    }
    return;
  } else {
    // pin changed
    pinLongPress = 0;
    pinChangeTime = t;
    if (pinState)
      topicQueue.put(eventPrefix + "state 1");
    else {
      topicQueue.put(eventPrefix + "state 0");
      if (!longtime) {
        topicQueue.put(eventPrefix + "click short");
        if (t - pinReleaseTime <= DOUBLECLICKTIME)
          topicQueue.put(eventPrefix + "click double");
        pinReleaseTime = t;
      }
    }
    if (pinIdle) {
      pinIdle = 0;
      topicQueue.put(eventPrefix + "idle 0");
    }
    lastPinState = pinState;
  }
}

//...............................................................................
//  input software debouncer
//...............................................................................
int GPIOinput::getInputState() {
  // combine with hardware debouncer (100 nF capacitor from gpioPin to GND)
  int lastpinState = pinState;
  pinState = digitalRead(pin);

  unsigned long now = millis();
  if (pinState != lastpinState)
    lastDebounceTime = now;
  if (now - lastDebounceTime > DEBOUNCETIME)
    return !pinState; // inverse logic
  else
    return -1; // undecided, still bouncing
}

//-------------------------------------------------------------------------------
//  GPIOinput private
//-------------------------------------------------------------------------------
//...............................................................................
// IRQ
//...............................................................................
void GPIOinput::irq() {
  irqDetected = 1;
}

//###############################################################################

//===============================================================================
//  GPIO output
//===============================================================================
GPIOoutput::GPIOoutput(string name, LOGGING &logging, TopicQueue &topicQueue,
                       int GPIOoutputPin)
    : Module(name, logging, topicQueue), pin(GPIOoutputPin) {}

//-------------------------------------------------------------------------------
//  GPIOoutput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOoutput::start() {

  Module::start();
  logging.info("setting GPIO pin " + String(pin) + " for output");
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

//...............................................................................
// handle
//...............................................................................
void GPIOoutput::handle() {
  Module::handle();

  String eventPrefix= "~/event/device/"+String(name)+" ";
  unsigned long now = millis();

  if (currentOutputMode == ON) {
    if (!currentOutputState) { // if is OFF
      digitalWrite(pin, HIGH);
      currentOutputState = HIGH;
      topicQueue.put(eventPrefix+"1");
    }
  } else if (currentOutputMode == OFF) {
    if (currentOutputState) { // if is ON
      digitalWrite(pin, LOW);
      currentOutputState = LOW;
      topicQueue.put(eventPrefix+"0");
    }
  } else if (currentOutputMode == BLINK) {
    // use arg[2] for frequency
    int outputOn = (now / currentOutputBlinkTime) % 2;
    if (currentOutputState != outputOn) {
      digitalWrite(pin, outputOn);
      currentOutputState = outputOn;
      String strState = "";
      if (outputOn)
        topicQueue.put(eventPrefix+"1");
      else
        topicQueue.put(eventPrefix+"0");
    }
  } else if (currentOutputMode == OFT) {
    if (!currentOutputState) { // if is OFF
      lastOftOnTime = now;
      digitalWrite(pin, HIGH);
      currentOutputState = HIGH;
      topicQueue.put(eventPrefix+"1");
    } else {
      if ((lastOftOnTime + currentOutputOFTtime) < now) {
        digitalWrite(pin, LOW);
        currentOutputState = LOW;
        currentOutputMode = OFF;
        topicQueue.put(eventPrefix+"0");
      }
    }
  }
}

//...............................................................................
//  output mode setter
//...............................................................................
void GPIOoutput::setOutputMode(outputMode_t mode, int t) {

  currentOutputMode = mode;
  String modeStr = "";
  if (currentOutputMode == ON)
    modeStr = "on";
  else if (currentOutputMode == OFF)
    modeStr = "off";
  else if (currentOutputMode == BLINK) {
    currentOutputBlinkTime = (t > 0 ? t : BLINKTIME);
    modeStr = "blink";
  } else if (currentOutputMode == OFT) {
    currentOutputOFTtime = t;
    modeStr = "oft";
  }

  logging.debug("set GPIO " + String(name) + " output mode to " + modeStr +
   " ("+String(t)+" ms)");
}

//-------------------------------------------------------------------------------
//  GPIOoutput private
//-------------------------------------------------------------------------------
