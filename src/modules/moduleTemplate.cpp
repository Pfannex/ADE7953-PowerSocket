#include "modules/moduleTemplate.h"
#include <Arduino.h>

//===============================================================================
//  GPIOinputX
//===============================================================================
GPIOinputX::GPIOinputX(string name, LOGGING &logging, TopicQueue &topicQueue)
          : module(name, logging, topicQueue)
          {
  pin=12;
}

//-------------------------------------------------------------------------------
//  GPIOinput public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void GPIOinputX::start() {

  module::start();
  logging.info("setting GPIO pin " + String(pin) + " for input");
  pinMode(pin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(pin), std::bind(&GPIOinput::irq, this), RISING);
}

//...............................................................................
// handle
//...............................................................................
void GPIOinputX::handle() {
/*
  while (irqDetected > 0){
    detachInterrupt(pin);
    irqDetected--;
    logging.debug("IRQ");
    attachInterrupt(digitalPinToInterrupt(pin), std::bind(&GPIOinputX::irq, this), RISING);
  }
*/

  module::handle();

}
