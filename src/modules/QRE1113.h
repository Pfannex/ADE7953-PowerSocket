#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"

//###############################################################################
//  QRE1113
//###############################################################################

#define QRE1113_Name    "module::QRE1113"
#define QRE1113_Version "0.1.0"

//###############################################################################
//  QRE1113 input
//###############################################################################
// threshold to detect ON
#define THRESHOLD_ON 500
// threshold to detect OFF
#define THRESHOLD_OFF 500
// timeout maximum RC-loading time
#define TIMEOUT 3000


class QRE1113 : public Module {

public:
  QRE1113(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOinputPin);
  int pin;

  void start();
  void handle();
  String getVersion();

private:
  int readQRE();                         // debouncer and state detector
  int pinState = 0;                      // the last pin state
};
