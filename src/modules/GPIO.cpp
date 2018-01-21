#include "GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================
GPIO::GPIO(LOGGING &logging, TopicQueue &topicQueue)
    : logging(logging), topicQueue(topicQueue) {}


//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIO::start() {
  logging.info("starting GPIO");

  pinMode(IN01, INPUT_PULLUP);
  pinMode(IN02, INPUT_PULLUP);
  pinMode(OUT01, OUTPUT);
  pinMode(OUT02, OUTPUT);
  digitalWrite(OUT01, LOW);
  digitalWrite(OUT02, LOW);

  for (int i = 0; i<17; i++){
    pinState[i] = 0;
    lastDebounceTime[i] = 0;
    lastPinState[i] = 0;
    pinChangeTime[i] = 0;
    pinIdle[i] = 0;
    pinLongPress[i] = 0;
    pinReleaseTime[i] = 0;

    currentOutputMode[i] = OFF;
    currentOutputState[i] = 0;
    lastOftOnTime[i] = 0;
  }
}

//...............................................................................
// handle
//...............................................................................
void GPIO::handle() {
  handleInput(IN01);
  handleInput(IN02);
  handleOutput(OUT01);
  handleOutput(OUT02);
}

//...............................................................................
//  GPIO set
//...............................................................................
String GPIO::set(Topic &topic) {
  /*
  ~/set
    └─device
      └─gpio [pin], [mode](on, off, blink, oft), [oftTime]
  */
  logging.debug("GPIO set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  //set gpio mode
  int pin = 0;
  outputMode_t mode = OFF;
  int oftTime = 0;

  if (topic.itemIs(3, "gpio")) {
    if (topic.getArgCount() > 1){
      pin = topic.getArgAsLong(0);
      if      (topic.argIs(1, "on"))    {mode = ON;}
      else if (topic.argIs(1, "off"))   {mode = OFF;}
      else if (topic.argIs(1, "blink")) {mode = BLINK;}
      else if (topic.argIs(1, "oft") and topic.getArgCount() > 2) {
        mode = OFT;
        oftTime = topic.getArgAsLong(2);
      }else{
        return TOPIC_NO;
      }
    }
    setOutputMode(pin, mode, oftTime);
    return TOPIC_OK;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
String GPIO::get(Topic &topic) {
  /*
  ~/get
    └─device
      └─gpio
        └─led (on, off, blink)
  */

  logging.debug("GPIO get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  GPIO get
//...............................................................................
void GPIO::on_events(Topic &topic){
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//  handle button events
//-------------------------------------------------------------------------------
void GPIO::handleInput(int gpioPin) {
  unsigned long now = millis();
  int pinState = getInputState(gpioPin);
  if (pinState < 0)
    return; // still bouncing

  String IO = String(gpioPin);
  unsigned long t = now;
  unsigned long tl = t - pinChangeTime[gpioPin];
  int idling = (tl >= IDLETIME);
  int longtime = (tl >= LONGPRESSTIME);

  // note the difference:
  // the short event is created when the button is released after a short time
  // the long event is created when the button is pressed for a long time
  if (pinState == lastPinState[gpioPin]) {
    // pin unchanged
    if (pinState) {
      // pin is pressed
      if (!pinLongPress[gpioPin] && longtime) {
        pinLongPress[gpioPin] = 1;
        topicQueue.put("~/event/gpio/" + IO + "/click long");
      }
    } else {
      // pin is not pressed
      if (!pinIdle && idling) {
        pinIdle[gpioPin] = 1;
        topicQueue.put("~/event/gpio/" + IO + "/idle 1");
      }
    }
    return;
  } else {
    // pin changed
    pinLongPress[gpioPin] = 0;
    pinChangeTime[gpioPin] = t;
    if (pinState)
      topicQueue.put("~/event/gpio/" + IO + "/state 1");
    else {
      topicQueue.put("~/event/gpio/" + IO + "/state 0");
      if(!longtime) {
        topicQueue.put("~/event/gpio/" + IO + "/click short");
        if(t-pinReleaseTime[gpioPin] <= DOUBLECLICKTIME)
          topicQueue.put("~/event/gpio/" + IO + "/click double");
        pinReleaseTime[gpioPin]= t;
      }
    }
    if (pinIdle[gpioPin]) {
      pinIdle[gpioPin] = 0;
      topicQueue.put("~/event/gpio/" + IO + "/idle 0");
    }
    lastPinState[gpioPin] = pinState;
  }
}

//...............................................................................
//  input software debouncer
//...............................................................................
int GPIO::getInputState(int gpioPin) {
  // combine with hardware debouncer (100 nF capacitor from gpioPin to GND)
  int lastpinState = pinState[gpioPin];
  pinState[gpioPin] = digitalRead(gpioPin);

  unsigned long now = millis();
  if (pinState[gpioPin] != lastpinState)
    lastDebounceTime[gpioPin] = now;
  if (now - lastDebounceTime[gpioPin] > DEBOUNCETIME)
    return !pinState[gpioPin]; // inverse logic
  else
    return -1; // undecided, still bouncing
}

//...............................................................................
//  output mode setter
//...............................................................................
void GPIO::setOutputMode(int gpioPin, outputMode_t mode, int t){
  String IO = String(gpioPin);

  currentOutputMode[gpioPin] = mode;
  currentOutputOFTtime[gpioPin] = t;
  String modeStr = "";
  if (currentOutputMode[gpioPin] == ON)          modeStr = "on";
  else if (currentOutputMode[gpioPin] == OFF)    modeStr = "off";
  else if (currentOutputMode[gpioPin] == BLINK)  modeStr = "blink";
  else if (currentOutputMode[gpioPin] == OFT)    modeStr = "oft";

  topicQueue.put("~/event/gpio/" + IO + " " + modeStr);
  logging.debug("GPIO set outputmode GPIO " + IO + " to " + modeStr);

}
//...............................................................................
//  handle output
//...............................................................................
void GPIO::handleOutput(int gpioPin){
  unsigned long now = millis();
  String IO = String(gpioPin);

  if (currentOutputMode[gpioPin] == ON){
    if (!currentOutputState[gpioPin]){  //if is OFF
      digitalWrite(gpioPin, HIGH);
      currentOutputState[gpioPin] = HIGH;
      topicQueue.put("~/event/gpio/" + IO + " is_on");
    }
  }else if (currentOutputMode[gpioPin] == OFF){
    if (currentOutputState[gpioPin]){  //if is ON
      digitalWrite(gpioPin, LOW);
      currentOutputState[gpioPin] = LOW;
      topicQueue.put("~/event/gpio/" + IO + " is_off");
    }
  }else if (currentOutputMode[gpioPin] == BLINK){
    //use arg[2] for frequency
    int outputOn = (now / BLINKTIME) % 2;
    if (currentOutputState[gpioPin] != outputOn){
      digitalWrite(gpioPin, outputOn);
      currentOutputState[gpioPin] = outputOn;
      String strState = "";
      if (outputOn) strState = " is_on"; else strState = " is_off";
      topicQueue.put("~/event/gpio/" + IO + strState);
    }
  }else if (currentOutputMode[gpioPin] == OFT){
    if (!currentOutputState[gpioPin]){ //if is OFF
      lastOftOnTime[gpioPin] = now;
      digitalWrite(gpioPin, HIGH);
      currentOutputState[gpioPin] = HIGH;
      topicQueue.put("~/event/gpio/" + IO + " is_on");
    }else{
      if ((lastOftOnTime[gpioPin] + currentOutputOFTtime[gpioPin]) < now){
        digitalWrite(gpioPin, LOW);
        currentOutputState[gpioPin] = LOW;
        currentOutputMode[gpioPin] = OFF;
        topicQueue.put("~/event/gpio/" + IO + " is_off");
      }
    }
  }
}
