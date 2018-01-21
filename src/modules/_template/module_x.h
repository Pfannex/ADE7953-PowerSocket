#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"

//###############################################################################
//  GPIO
//###############################################################################

class module_x {

public:
  module_x(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

};
