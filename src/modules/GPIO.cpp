#include "GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIOinput
//===============================================================================
GPIOinput::GPIOinput(int GPIOinputPin, LOGGING &logging, TopicQueue &topicQueue)
          :pin(GPIOinputPin), logging(logging), topicQueue(topicQueue){
}


//-------------------------------------------------------------------------------
//  GPIOinput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOinput::start() {

  logging.info("starting GPIOinput for Pin " + String(pin));
  pinMode(pin, INPUT_PULLUP);

}

//...............................................................................
// handle
//...............................................................................
void GPIOinput::handle() {
  unsigned long now = millis();
  int pinState = getInputState();
  if (pinState < 0)
    return; // still bouncing

  String IO = String(pin);
  unsigned long t = now;
  unsigned long tl = t - pinChangeTime;
  int idling = (tl >= IDLETIME);
  int longtime = (tl >= LONGPRESSTIME);

  // note the difference:
  // the short event is created when the button is released after a short time
  // the long event is created when the button is pressed for a long time
  if (pinState == lastPinState) {
    // pin unchanged
    if (pinState) {
      // pin is pressed
      if (!pinLongPress && longtime) {
        pinLongPress = 1;
        topicQueue.put("~/event/gpio/" + IO + "/click long");
      }
    } else {
      // pin is not pressed
      if (!pinIdle && idling) {
        pinIdle = 1;
        topicQueue.put("~/event/gpio/" + IO + "/idle 1");
      }
    }
    return;
  } else {
    // pin changed
    pinLongPress = 0;
    pinChangeTime = t;
    if (pinState)
      topicQueue.put("~/event/gpio/" + IO + "/state 1");
    else {
      topicQueue.put("~/event/gpio/" + IO + "/state 0");
      if(!longtime) {
        topicQueue.put("~/event/gpio/" + IO + "/click short");
        if(t-pinReleaseTime <= DOUBLECLICKTIME)
          topicQueue.put("~/event/gpio/" + IO + "/click double");
        pinReleaseTime = t;
      }
    }
    if (pinIdle) {
      pinIdle = 0;
      topicQueue.put("~/event/gpio/" + IO + "/idle 0");
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
void GPIOinput::on_events(Topic &topic){
  //Serial.println(topic.asString());
}
//-------------------------------------------------------------------------------
//  GPIOinput private
//-------------------------------------------------------------------------------
