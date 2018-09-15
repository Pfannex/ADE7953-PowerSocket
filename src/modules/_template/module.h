#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include <FunctionalInterrupt.h>

//###############################################################################
//  GPIO
//###############################################################################

#define myModule_Name    "module::myModule"
#define myModule_Version "0.1.0"

//###############################################################################
//  myModule
//###############################################################################
// threshold to detect ON
#define MYDEFINE 500

class myModule : public Module {

public:
  myModule(string name, LOGGING &logging, TopicQueue &topicQueue);

  void start();
  void handle();
  String getVersion();

private:
};
