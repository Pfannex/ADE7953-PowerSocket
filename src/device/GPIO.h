#include "Logger.h"
#include "Setup.h"
#include "Topic.h"
#include "DeviceSetup.h"

//###############################################################################
//  GPIO
//###############################################################################

class GPIO {

public:
  GPIO(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;
};
