#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO
//###############################################################################

class MCP23017 {

public:
  MCP23017(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  void irq();
  int irqDetected = 0;

};
