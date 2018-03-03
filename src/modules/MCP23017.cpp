#include "MCP23017.h"
#include <Arduino.h>

//===============================================================================
//  MCP23017
//===============================================================================
MCP23017::MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOIrqPin)
         :Module(name, logging, topicQueue), irqPin(GPIOIrqPin)
         {}


//-------------------------------------------------------------------------------
//  MCP23017 public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void MCP23017::start() {
  Module::start();
  logging.info("setting GPIO pin " + String(irqPin) + " to IRQ-Pin");
  pinMode(irqPin, INPUT_PULLUP);
  irqSetMode(FALLING);
}

//...............................................................................
// handle
//...............................................................................
void MCP23017::handle() {
  Module::handle();
  irqHandle();
}

//...............................................................................
// getVersion
//...............................................................................
String MCP23017::getVersion() {
  return  String(MCP23017_Name) + " v" + String(MCP23017_Version);
}

//...............................................................................
// IRQ
//...............................................................................
void MCP23017::irq() {
  irqDetected = 1;
}

void MCP23017::irqSetMode(int mode){
  if (mode == 4){
    detachInterrupt(irqPin);
  }else{
    //LOW     = 0, HIGH    = 1, RISING  = 1, FALLING = 2
    //CHANGE  = 3, OFF     = 4
    attachInterrupt(digitalPinToInterrupt(irqPin), std::bind(&MCP23017::irq, this), mode);
  }
}

void MCP23017::irqHandle() {
  unsigned long now = millis();
  String eventPrefix= "~/event/device/" + String(name) + "/";

// main handling
  if (irqDetected){
    irqSetMode(irqOFF);
    irqDetected = 0;
    logging.debug("MCP23017 IRQ");

    //idle handling
  }
}
//-------------------------------------------------------------------------------
//  MCP23017 private
//-------------------------------------------------------------------------------
