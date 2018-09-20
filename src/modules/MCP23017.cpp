#include "MCP23017.h"
#include <Arduino.h>

//===============================================================================
//  MCP23017
//===============================================================================
MCP23017::MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue,
                   int GPIOIrqPin, int sda, int scl)
         :Module(name, logging, topicQueue),
          irqPin(GPIOIrqPin), sda(sda), scl(scl)
         {}

//-------------------------------------------------------------------------------
//  MCP23017 public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void MCP23017::start() {
  Module::start();
  Wire.begin(sda, scl);
  Wire.i2c.i2cAddr = 0x20;
  logging.info("setting GPIO pin " + String(irqPin) + " to IRQ-Pin");

  pinMode(irqPin, INPUT_PULLUP);
  irqSetMode(FALLING);

  // clear Outputs
  Wire.i2c.write8_8(MCP23017_GPIOA, 0x00);
  Wire.i2c.write8_8(MCP23017_GPIOB, 0x00);
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
  irqDetected = true;
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
    irqDetected = false;
    lastIrqTime = now;

    // find pin
    uint8_t pin = mcp.getLastInterruptPin();
    uint8_t val = mcp.getLastInterruptPinValue();
    uint8_t state = mcp.digitalRead(pin);

    logging.debug("mcpGPIO " + String(pin) + " | " + String(val));
    topicQueue.put(eventPrefix + "/" + String(pin) + " " + String(val));

    //sledge hammer
    Wire.i2c.read8_8(MCP23017_GPIOA);
    Wire.i2c.read8_8(MCP23017_GPIOB);

    delay(1);
    irqSetMode(FALLING);

    clearIRQ();

  }

//debouncing
/*
  if (now - lastIrqTime > MCPDEBOUNCETIME && lastIrqTime > 0){
    logging.debug("debounce enable IRQ");
    irqDetected = false;  //clear accrued IRQs during debouncing
    lastIrqTime = 0;

  }
*/

}
//-------------------------------------------------------------------------------
//  MCP23017 private
//-------------------------------------------------------------------------------

//...............................................................................
// clear MCP23017 IRQ
//...............................................................................
void MCP23017::clearIRQ() {

  while (Wire.i2c.read8_8(MCP23017_INTFA) > 0 || Wire.i2c.read8_8(MCP23017_INTFB) > 0){
    logging.error("MCP23017 IRQ is hanging.....");
    Wire.i2c.read8_8(MCP23017_GPIOA);
    Wire.i2c.read8_8(MCP23017_GPIOB);
  }
}
