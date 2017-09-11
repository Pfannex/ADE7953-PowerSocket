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
  currentButtonMode.t = 0;
  currentButtonMode.idle = 0;
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
  if(value)
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
  if(value)
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

  // on button press:
  // - do nothing, start time measurement
  // on button release:
  // - if in long mode leave long mode if less than LONGPRESSTIME has passed
  // - if not in long mode toggle short mode
  // on idling:
  // - leave long mode

  unsigned long tl = now - currentButtonMode.t;
  int idling = (tl >= IDLETIME);
  int longtime = (tl >= LONGPRESSTIME);

  buttonMode_t mode = currentButtonMode;
  mode.state = !buttonPinState; // down if pin is 0
  if (mode.state != currentButtonMode.state) {
    // button changed
    mode.t = now;
    /*logging.debug("Button changed to " + String(mode.state) + " @ " +
                  String(mode.t));*/
    // handle button press or release
    if (!mode.state) {
      // button up
      /*logging.debug("button released, time=" + String(tl) +
                    " longtime=" + String(longtime));*/
      if (mode.L) {
        if (!longtime)
          mode.L = 0;
      } else
        mode.S = !mode.S;
    } else {
      // button down
      mode.L = 0; // drop out of long mode
    }
    mode.idle = 0;
    setButtonMode(mode);
  } else {
    // button did not change
    if (!mode.L && mode.state && longtime) {
      // not in long mode and button pressed for a long time
      mode.L = 1; // this sets long mode even before the button is released
      mode.idle = 0;
      setButtonMode(mode);
    } else {
      if (!mode.idle && idling) {
        mode.idle = 1;
        setButtonMode(mode);
      }
    }
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
    return buttonPinState[buttonPin];
  else
    return -1; // undecided, still bouncing
}

void GPIO::printButtonMode(String msg, buttonMode_t mode) {
  logging.debug(msg + ": Button mode S= " + String(mode.S) +
                " L= " + String(mode.L) + " button= " + String(mode.state) +
                " idle= " + String(mode.idle));
}

void GPIO::setButtonMode(buttonMode_t mode) {

  buttonMode_t oldMode= currentButtonMode;
  buttonMode_t newMode= mode;

  // printButtonMode("old", oldMode);
  // printButtonMode("new", newMode);

  // change in button (pressed or released)
  if (newMode.state != oldMode.state) {
    // queue button event
    if (newMode.state)
      topicQueue.put("~/event/gpio/button/down 1");
    else
      topicQueue.put("~/event/gpio/button/down 0");
  }

  // change in short mode
  if (newMode.S != oldMode.S) {
    // enqueue change in short mode
    if (newMode.S) {
      topicQueue.put("~/event/gpio/button/short 1");
    } else {
      topicQueue.put("~/event/gpio/button/short 0");
    }
    // the controller will decide what to do with the event
  }

  // enter idling
  if (newMode.idle) {
    if (!oldMode.idle) {
      // enter idle mode
      topicQueue.put("~/event/gpio/button/idle 1");
      // drop out of long mode
      newMode.L = 0;
    }
  } else {
    if (oldMode.idle) {
      // leave idle mode
      topicQueue.put("~/event/gpio/button/idle 0");
    }
  }

  // change in long mode
  if (newMode.L != oldMode.L) {
    // enqueue change in long mode
    if (newMode.L) {
      topicQueue.put("~/event/gpio/button/long 1");
    } else {
      topicQueue.put("~/event/gpio/button/long 0");
    }
  };

  currentButtonMode = mode;
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
