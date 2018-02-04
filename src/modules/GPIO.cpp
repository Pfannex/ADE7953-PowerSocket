#include "modules/GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIOinput
//===============================================================================
GPIOinput::GPIOinput(string name, LOGGING &logging, TopicQueue &topicQueue,
          int GPIOinputPin)
          : module(name, logging, topicQueue), pin(GPIOinputPin)
          {
}

//-------------------------------------------------------------------------------
//  GPIOinput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOinput::start() {

  module::start();
  logging.info("setting GPIO pin " + String(pin) + " for input");
  pinMode(pin, INPUT_PULLUP);
}

//...............................................................................
// handle
//...............................................................................
void GPIOinput::handle() {

  module::handle();

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

//...............................................................................
//  GPIOinput set
//...............................................................................
String GPIOinput::set(Topic &topic) {
  /*
  ~/set
    └─GPIOinput
        └─led (on, off, blink)
  */

  logging.debug("GPIOinput set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIOinput get
//...............................................................................
String GPIOinput::get(Topic &topic) {
  /*
  ~/get
    └─GPIOinput
        └─led (on, off, blink)
  */

  logging.debug("GPIOinput get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIOinput get
//...............................................................................
void GPIOinput::on_events(Topic &topic) {
  // Serial.println(topic.asString());
}
//-------------------------------------------------------------------------------
//  GPIOinput private
//-------------------------------------------------------------------------------

//###############################################################################

//===============================================================================
//  GPIO output
//===============================================================================
GPIOoutput::GPIOoutput(string name, LOGGING &logging, TopicQueue &topicQueue,
                       int GPIOoutputPin)
    : module(name, logging, topicQueue), pin(GPIOoutputPin) {}

//-------------------------------------------------------------------------------
//  GPIOoutput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOoutput::start() {

  module::start();
  logging.info("setting GPIO pin " + String(pin) + " for output");
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

//...............................................................................
// handle
//...............................................................................
void GPIOoutput::handle() {
  module::handle();

  String eventPrefix= "~/event/device/"+String(name)+" ";
  unsigned long now = millis();

  if (currentOutputMode == ON) {
    if (!currentOutputState) { // if is OFF
      digitalWrite(pin, HIGH);
      currentOutputState = HIGH;
      topicQueue.put(eventPrefix+"is_on");
    }
  } else if (currentOutputMode == OFF) {
    if (currentOutputState) { // if is ON
      digitalWrite(pin, LOW);
      currentOutputState = LOW;
      topicQueue.put(eventPrefix+"is_off");
    }
  } else if (currentOutputMode == BLINK) {
    // use arg[2] for frequency
    int outputOn = (now / currentOutputBlinkTime) % 2;
    if (currentOutputState != outputOn) {
      digitalWrite(pin, outputOn);
      currentOutputState = outputOn;
      String strState = "";
      if (outputOn)
        topicQueue.put(eventPrefix+"is_on");
      else
        topicQueue.put(eventPrefix+"is_off");
    }
  } else if (currentOutputMode == OFT) {
    if (!currentOutputState) { // if is OFF
      lastOftOnTime = now;
      digitalWrite(pin, HIGH);
      currentOutputState = HIGH;
      topicQueue.put(eventPrefix+"is_on");
    } else {
      if ((lastOftOnTime + currentOutputOFTtime) < now) {
        digitalWrite(pin, LOW);
        currentOutputState = LOW;
        currentOutputMode = OFF;
        topicQueue.put(eventPrefix+"is_off");
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

//...............................................................................
//  GPIOoutput set
//...............................................................................
String GPIOoutput::set(Topic &topic) {
  /*
  ~/set
    └─device
      └─gpio [pin], [mode](on, off, blink, oft), [oftTime]
  */
  logging.debug("GPIO set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  // set gpio mode
  String strPin = topic.getArg(0);
  outputMode_t mode = OFF;
  int oftTime = 0;

  if (topic.itemIs(3, "gpio")) {
    if (topic.getArgCount() > 1 and strPin.toInt() == pin) {
      if (topic.argIs(1, "on")) {
        mode = ON;
      } else if (topic.argIs(1, "off")) {
        mode = OFF;
      } else if (topic.argIs(1, "blink")) {
        mode = BLINK;
      } else if (topic.argIs(1, "oft") and topic.getArgCount() > 2) {
        mode = OFT;
        oftTime = topic.getArgAsLong(2);
      } else {
        return TOPIC_NO;
      }
      setOutputMode(mode, oftTime);
      return TOPIC_OK;
    } else {
      return TOPIC_NO;
    }
  }
}
//...............................................................................
//  GPIOoutput get
//...............................................................................
String GPIOoutput::get(Topic &topic) {
  /*
  ~/get
    └─GPIOoutput
        └─led (on, off, blink)
  */

  logging.debug("GPIOoutput get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIOinput get
//...............................................................................
void GPIOoutput::on_events(Topic &topic) {
  // Serial.println(topic.asString());
}
//-------------------------------------------------------------------------------
//  GPIOoutput private
//-------------------------------------------------------------------------------
