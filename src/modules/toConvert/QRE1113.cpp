#include "QRE1113.h"
#include <Arduino.h>

//===============================================================================
//  QRE1113
//===============================================================================
QRE1113::QRE1113(int QRE1113Pin, LOGGING &logging, TopicQueue &topicQueue)
          :pin(QRE1113Pin), logging(logging), topicQueue(topicQueue){
}


//-------------------------------------------------------------------------------
//  QRE1113 public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void QRE1113::start() {

  logging.info("starting QRE1113 for Pin " + String(pin));
  pinMode(pin, OUTPUT);

}

//...............................................................................
// handle
//...............................................................................
void QRE1113::handle() {

  String IO = String(pin);
  int QRE_Value = readQRE();

  //Serial.println(QRE_Value);
  //Serial.println(pinState);

  if (QRE_Value > 500 && !pinState){
    pinState = 1;
    topicQueue.put("~/event/qre1113/" + IO + "/state on");
    //digitalWrite( 14, LOW );
  }else if (QRE_Value < 500 && pinState){
    pinState = 0;
    topicQueue.put("~/event/qre1113/" + IO + "/state off");
    //digitalWrite( 14, HIGH );
  }

/*
  unsigned long now = millis();
  int pinState = getInputState();
  if (pinState < 0)
    return; // still bouncing
*/


/*
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
*/
}

//...............................................................................
//  input software debouncer
//...............................................................................
int QRE1113::readQRE() {

  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  pinMode(pin, INPUT );

  long time = micros();

  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  int diff = 0;
  while (digitalRead(pin) == HIGH and diff < 3000){
    diff = micros() - time;
  }
  return diff;

/*
  int lastpinState = pinState;
  pinState = digitalRead(pin);

  unsigned long now = millis();
  if (pinState != lastpinState)
    lastDebounceTime = now;
  if (now - lastDebounceTime > DEBOUNCETIME)
    return !pinState; // inverse logic
  else
    return -1; // undecided, still bouncing
*/
}


//...............................................................................
//  QRE1113 set
//...............................................................................
String QRE1113::set(Topic &topic) {
  /*
  ~/set
    └─QRE1113
        └─led (on, off, blink)
  */

  logging.debug("QRE1113 set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  QRE1113 get
//...............................................................................
String QRE1113::get(Topic &topic) {
  /*
  ~/get
    └─QRE1113
        └─led (on, off, blink)
  */

  logging.debug("QRE1113 get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  String strPin = topic.getArg(0);


  if (topic.itemIs(3, "qre1113")) {
    Serial.println(topic.getArgCount());
    Serial.println(strPin.toInt());
    Serial.println(pin);
    if (strPin.toInt() == pin){
      if (pinState == 1) return "on";
      if (pinState == 0) return "off";
    }
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  QRE1113 get
//...............................................................................
void QRE1113::on_events(Topic &topic){
  //Serial.println(topic.asString());
}
//-------------------------------------------------------------------------------
//  QRE1113 private
//-------------------------------------------------------------------------------
