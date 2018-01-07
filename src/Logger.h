#pragma once
#include "Arduino.h"
#include "Clock.h"
#include "Debug.h"

//###############################################################################
//  LOGGING
//###############################################################################

#include <functional>
typedef std::function<void(const String&, const String&)> LogFunction;


class LOGGING{
public:
  LOGGING(Clock& clock);
  Clock& clock;

  void start();

  void log(const String &channel, const String &msg);

  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);

  void setLogFunction(const LogFunction lf);

private:
  LogFunction logFunction;
};
