#include "framework/Utils/Logger.h"
#include "Setup.h"
#include <Arduino.h>
#include "framework/Topic.h"
#include "framework/Utils/Strings.h"

//###############################################################################
//  module
//###############################################################################

class module {

public:
  module(string name, LOGGING &logging, TopicQueue &topicQueue);
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
