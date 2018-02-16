#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/Topic.h"
#include <Arduino.h>
#include <Wire.h> //I²C

//###############################################################################
//  I2C
//###############################################################################

class I2C {

public:
  I2C(int sda, int scl, LOGGING &logging, TopicQueue &topicQueue);
  int sda;
  int scl;
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);

  void scanBus();

private:
  LOGGING &logging;
  TopicQueue &topicQueue;

};
