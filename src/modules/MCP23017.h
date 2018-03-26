#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>
#include "Adafruit_MCP23017.h"

//###############################################################################
//  GPIO
//###############################################################################

#define MCP23017_Name    "module::MCP23017"
#define MCP23017_Version "0.1.0"

//###############################################################################
//  GPIO
//###############################################################################
// time in ms pin must be stable before reporting change
#define MCPDEBOUNCETIME 500

class MCP23017 : public Module {

public:
  MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue,
           int GPIOIrqPin, int sda, int scl);
  int irqPin;
  int sda;
  int scl;

  void start();
  void handle();
  String getVersion();

  Adafruit_MCP23017 mcp;

private:
  void configMCP();
  void clearIRQ();
  void irq();                         // irq jumpTo funktion
  void irqHandle();                   // handle irq function
  #define irqOFF 4                    // irq detachInterrupt mode
  void irqSetMode(int mode);          // irq mode setter
  volatile bool irqDetected = false;
  unsigned long lastIrqTime = 0;
};
