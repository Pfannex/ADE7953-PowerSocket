#pragma once
#include "modules/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO
//###############################################################################

#define MCP23017_Name    "module::MCP23017"
#define MCP23017_Version "0.1.0"

//###############################################################################
//  GPIO
//###############################################################################

class MCP23017 : public Module {

public:
  MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOIrqPin);
  int irqPin;

  void start();
  void handle();
  String getVersion();

private:
  void irq();                       // irq jumpTo funktion
  void irqHandle();                 // handle irq function
  #define irqOFF 4                  // irq detachInterrupt mode
  void irqSetMode(int mode);        // irq mode setter
  int irqDetected = 0;
};
