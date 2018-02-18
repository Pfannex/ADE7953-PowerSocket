#include "framework/Utils/Logger.h"
#include "Setup.h"
#include <Arduino.h>
#include "framework/Topic.h"
#include "framework/Utils/Strings.h"

/*
A module represents a physical part in the device. For example, modules can
be GPIOs, LCDs, A/D converts etc. Several modules can be present in a device.
A module has a name to discern it from other modules of the same type in the
device.

The single modules are not controlled by set and get directly. The device
class controls them by invoking module-specific methods. The modules inform
the framework about its state by putting events into the topic queue.

*/

//###############################################################################
//  module
//###############################################################################

class Module {

public:
  Module(string name, LOGGING &logging, TopicQueue &topicQueue);
  virtual void start();
  virtual void handle();
  // check if topic is .../.../device/<name>
  bool isForModule(Topic &topic);
  // check if topic is .../.../device/<name>/<item>
  bool isItem(Topic &topic, string item);

protected:
  string name; // name could be multipart, we only allow single part
  int nameItemCount= 1; // number items in name, e.g. 1 for "relay"
  LOGGING &logging;
  TopicQueue &topicQueue;

};
