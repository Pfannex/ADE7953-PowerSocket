#include "GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================

//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
GPIO::GPIO(LOGGING &logging) : logging(logging) {}

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
// handle
//...............................................................................

void GPIO::handle() {

#ifdef HAS_BUTTON
  // how to use the buttonMode?
  // - state signals if the button is up (0) or down (1)
  // - statex signals if state has been the same for at least BTIME milliseconds
  // - a short press of the button toggles the modeS between off (0) and on (1)
  // - a long press of the button toggles the modeL between off (0) and on (1)

  int buttonPinState = getButtonPinState(BUTTON_PIN);
  if (buttonPinState < 0)
    return; // still bouncing

  unsigned long now = millis();

  buttonMode_t mode = currentButtonMode;
  mode.state = !buttonPinState; // down if pin is 0
  if (mode.state != currentButtonMode.state) {
    // button changed
    mode.t = now;
    logging.debug("Button changed to "+
      String(mode.state)+" @ "+String(mode.t));
    if (!mode.state) {
      // button up
      if (mode.idle)
        mode.L = !mode.L;
      else
        mode.S = !mode.S;
    }
    mode.idle = 0;
    setButtonMode(mode);
  } else {
    if (!mode.idle && (now - mode.t >= BTIME)) {
      mode.idle = 1;
      setButtonMode(mode);
    }
  }
#endif
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
  logging.info(msg + ": Button mode S= " + String(mode.S) +
                 " L= " + String(mode.L) + " button= " + String(mode.state) +
                 " idle= " + String(mode.idle));
}

void GPIO::setButtonMode(buttonMode_t mode) {

  onSetButtonMode(currentButtonMode, mode);
  currentButtonMode = mode;
}
#endif

//...............................................................................
// LED routines
//...............................................................................

#ifdef HAS_LED
void GPIO::Led(int on) { digitalWrite(LED_PIN, on); }
#endif

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
// relay routines
//...............................................................................

#ifdef HAS_RELAY
void GPIO::Relay(int on) { digitalWrite(RELAY_PIN, on); }
#endif

//===> button mode service routine --------------------------------------------

void GPIO::onSetButtonMode(buttonMode_t oldMode, buttonMode_t newMode) {

  // examples for how to use the idle flag:
  // - if button is pressed, indicate that releasing the button will toggle the L mode
  // - leave an input mode when not button is pressed for some time

  /*printButtonMode("old", oldMode);
  printButtonMode("new", newMode);*/
  if(newMode.S != oldMode.S)  {
      // here we enqueue the event
      //queue.put("~/foo/button/shortPress")
      // the controller will decide what to do with the event
  }


  if(newMode.L != oldMode.L) {
    if(newMode.L)
      // queue.put("~/foo/button/long 1");
      Serial.println("God mode is on.");
    else
    // queue.put("~/foo/button/long 0");
      Serial.println("God mode is off.");
  } else {
    if(newMode.state && newMode.idle != oldMode.idle && newMode.idle) {
      if(newMode.L) {
        Serial.println("Prepare to leave God mode.");
        if(newMode.S) currentLedMode= ON; else currentLedMode= OFF;
      } else  {
        Serial.println("Prepare to enter God mode.");
        currentLedMode= BLINK;
      }
    }
  }
}

/*
void GPIO::switchRelay(int state) {
  if (state == 1){
    if(!currentButtonMode.L) currentLedMode= ON;   // signalisation of Mode L takes precedence
      Relay(1);
      pub(3,0, "on");
      Serial.println("Relay is on.");
    } else {
      if(!currentButtonMode.L) currentLedMode= OFF; // signalisation of Mode L takes precedence
      Relay(0);
      pub(3,0, "off");
     Serial.println("Relay is off.");
    }
}
*/
