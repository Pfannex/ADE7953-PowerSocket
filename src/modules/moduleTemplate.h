#include "modules/module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO input
//###############################################################################

class GPIOinputX : public module {

public:
  GPIOinputX(string name, LOGGING &logging, TopicQueue &topicQueue);
  int pin;

  void start();
  void handle();

private:

  //void irq();
  //int irqDetected = 0;
};
