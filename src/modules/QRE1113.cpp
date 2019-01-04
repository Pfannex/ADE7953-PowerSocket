#include "QRE1113.h"
#include <Arduino.h>

//===============================================================================
//  QRE1113
//===============================================================================
QRE1113::QRE1113(string name, LOGGING &logging, TopicQueue &topicQueue,
          int GPIOinputPin)
          : Module(name, logging, topicQueue), pin(GPIOinputPin)
          {}

//-------------------------------------------------------------------------------
//  QRE1113 public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void QRE1113::start() {
  Module::start();
  logging.info("setting GPIO pin " + String(pin) + " for QRE1113 input");
  pinMode(pin, OUTPUT);
}

//...............................................................................
// handle
//...............................................................................
void QRE1113::handle() {
  Module::handle();
  unsigned long now = millis();

  //debouncing and state detection
  if (now - lastChangeTime > DEBOUNCETIME){
    lastChangeTime = now;

    String IO = String(pin);
    int QRE_Value = readQRE();
    String eventPrefix= "~/event/device/" + String(name) + "/";

    if (QRE_Value > 500 && !pinState){
      pinState = 1;
      logging.debug("QRE1113 " + String(pin) + " open");
      topicQueue.put(eventPrefix + "/state 1");
      //digitalWrite( 14, LOW );
    }else if (QRE_Value < 500 && pinState){
      pinState = 0;
      logging.debug("QRE1113 " + String(pin) + " closed");
      topicQueue.put(eventPrefix + "/state 0");
    }
  }
}

//...............................................................................
// getVersion
//...............................................................................
String QRE1113::getVersion() {
  return  String(QRE1113_Name) + " v" + String(QRE1113_Version);
}

//...............................................................................
// return pinState
//...............................................................................
int QRE1113::state() {
  return pinState;
}

//-------------------------------------------------------------------------------
//  QRE1113 private
//-------------------------------------------------------------------------------
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
  while (digitalRead(pin) == HIGH and diff < TIMEOUT){
    diff = micros() - time;
  }
  return diff;
}
