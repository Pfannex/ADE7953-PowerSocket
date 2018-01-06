#include "GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================

//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
GPIO::GPIO(LOGGING &logging, TopicQueue &topicQueue)
    : logging(logging), topicQueue(topicQueue) {}

void GPIO::start() {

  logging.info("starting GPIO");
// pinMode(LED2_PIN, OUTPUT);
// pinMode(LED3_PIN, OUTPUT);

#ifdef HAS_BUTTON
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < 17; i++) {
    buttonPinState[i] = 0;
    lastDebounceTime[i] = 0;
  }
  lastButtonState = 0;
  buttonChangeTime = 0;
  buttonIdle = 0;

#endif

#ifdef HAS_LED
  pinMode(LED_PIN, OUTPUT);
#endif

#ifdef HAS_NEOPIXEL
  WS_LED.setBrightness(100);
  WS_LED.begin();
  WS_LED.show(); // Initialize all pixels to 'off'
#endif

#ifdef HAS_RELAY
  pinMode(RELAY_PIN, OUTPUT);
#endif
}

//...............................................................................
// setLedMode
//...............................................................................

void GPIO::setLedMode(ledMode_t ledMode) {

//
// LED
//

#ifdef HAS_LED
  currentLedMode = ledMode;
  if (currentLedMode == ON)
    topicQueue.put("~/event/gpio/led on");
  else if (currentLedMode == OFF)
    topicQueue.put("~/event/gpio/led off");
  else if (currentLedMode == BLINK)
    topicQueue.put("~/event/gpio/led blink");
#endif
}

//...............................................................................
// setNeoPixelMode
//...............................................................................

void GPIO::setNeoPixelMode(int value) {

//
// Neopixel
//

#ifdef HAS_NEOPIXEL
  Neopixel(value);
  if (value)
    topicQueue.put("~/event/gpio/neopixel 1");
  else
    topicQueue.put("~/event/gpio/neopixel 0");
#endif
}

//...............................................................................
// setRelayMode
//...............................................................................

void GPIO::setRelayMode(int value) {

//
// Relay
//

#ifdef HAS_RELAY
  Relay(value);
  if (value)
    topicQueue.put("~/event/gpio/relay 1");
  else
    topicQueue.put("~/event/gpio/relay 0");
#endif
}

//...............................................................................
// handle
//...............................................................................

void GPIO::handle() {

  unsigned long now = millis();

//
// LED
//

#ifdef HAS_LED
  int ledOn = (currentLedMode == ON);
  if (currentLedMode == BLINK) {
    ledOn = (now / BLINKTIME) % 2;
  }
  Led(ledOn);
#endif

#ifdef HAS_NEOPIXEL
  int neopixelOn = (currentLedMode == ON);
  if (currentLedMode == BLINK) {
    neopixelOn = (now / BLINKTIME) % 2;
  }
  Neopixel(neopixelOn);
#endif

//
// button
//

#ifdef HAS_BUTTON
  /* this can be replaced by interrupts:
  https://techtutorialsx.com/2016/12/11/esp8266-external-interrupts/
  */
  int buttonPinState = getButtonPinState(BUTTON_PIN);
  if (buttonPinState < 0)
    return; // still bouncing

  unsigned long t = now;
  unsigned long tl = t - buttonChangeTime;
  int idling = (tl >= IDLETIME);
  int longtime = (tl >= LONGPRESSTIME);

  // note the difference:
  // the short event is created when the button is released after a short time
  // the long event is created when the button is pressed for a long time
  if (buttonPinState == lastButtonState) {
    // button unchanged
    if (buttonPinState) {
      // button is pressed
      if (!buttonLongPress && longtime) {
        buttonLongPress = 1;
        topicQueue.put("~/event/gpio/button/click long");
      }
    } else {
      // button is not pressed
      if (!buttonIdle && idling) {
        buttonIdle = 1;
        topicQueue.put("~/event/gpio/button/idle 1");
      }
    }
    return;
  } else {
    // button changed
    buttonLongPress = 0;
    buttonChangeTime = t;
    if (buttonPinState)
      topicQueue.put("~/event/gpio/button/state 1");
    else {
      topicQueue.put("~/event/gpio/button/state 0");
      if(!longtime) {
        topicQueue.put("~/event/gpio/button/click short");
        if(t-buttonReleaseTime <= DOUBLECLICKTIME)
          topicQueue.put("~/event/gpio/button/click double");
        buttonReleaseTime= t;
      }
    }
    if (buttonIdle) {
      buttonIdle = 0;
      topicQueue.put("~/event/gpio/button/idle 0");
    }
    lastButtonState = buttonPinState;
  }

#endif
}

//...............................................................................
//  GPIO set
//...............................................................................

String GPIO::set(Topic &topic) {
  /*
  ~/set
    └─gpio
        └─led (on, off, blink)
  */

  logging.debug("GPIO set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "led")) {
    if (topic.argIs(0, "on"))
      currentLedMode == ON;
    else if (topic.argIs(0, "off"))
      currentLedMode = OFF;
    else if (topic.argIs(0, "blink"))
      currentLedMode = BLINK;
    else
      return TOPIC_NO;
    logging.debug("LED mode set");
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------

//...............................................................................
// button routines
//...............................................................................

#ifdef HAS_BUTTON
// software debouncer
int GPIO::getButtonPinState(int buttonPin) {
  // combine with hardware debouncer (100 nF capacitor from buttonPin to GND)
  int lastButtonPinState = buttonPinState[buttonPin];
  buttonPinState[buttonPin] = digitalRead(buttonPin);
  unsigned long now = millis();
  /* char s[128];
  sprintf(s, "%d(%d) -> %d(%d)", lastButtonPinState,
  lastDebounceTime[buttonPin],
    buttonPinState[buttonPin], now);
  Serial.println(s);   */
  if (buttonPinState[buttonPin] != lastButtonPinState)
    lastDebounceTime[buttonPin] = now;
  if (now - lastDebounceTime[buttonPin] > DEBOUNCETIME)
    return !buttonPinState[buttonPin]; // inverse logic
  else
    return -1; // undecided, still bouncing
}

#endif

//...............................................................................
// LED routines
//...............................................................................

#ifdef HAS_LED
void GPIO::Led(int on) { digitalWrite(LED_PIN, on); }
#endif

//...............................................................................
// Neoxpixel routines
//...............................................................................

#ifdef HAS_NEOPIXEL
void GPIO::Neopixel(int on) {

  if (on) {
    WS_LED.setPixelColor(0, WS_LED.Color(255, 0, 0));
    WS_LED.show();
    WS_LED.setPixelColor(1, WS_LED.Color(255, 0, 0));
    WS_LED.show();
  } else {
    WS_LED.setPixelColor(0, WS_LED.Color(0, 255, 0));
    WS_LED.show();
    WS_LED.setPixelColor(1, WS_LED.Color(0, 255, 0));
    WS_LED.show();
  }
}
#endif

//...............................................................................
// Relay routines
//...............................................................................

#ifdef HAS_RELAY
void GPIO::Relay(int on) { digitalWrite(RELAY_PIN, on); }
#endif
