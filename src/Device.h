#include "Logger.h"
#include "Setup.h"
#include "Topic.h"

//###############################################################################
//  Device
//###############################################################################

class Device {

public:
  Device(LOGGING &logging, TopicQueue &topicQueue);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

  //define device specific functions here #######################################
  
};
