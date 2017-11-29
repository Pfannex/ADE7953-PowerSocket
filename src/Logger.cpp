#include "Logger.h"
#include "SysUtils.h"

//###############################################################################
//  logging
//###############################################################################
LOGGING::LOGGING(Clock &clock) : clock(clock) {}

//-------------------------------------------------------------------------------
//  LOGGING public
//-------------------------------------------------------------------------------

//...............................................................................
//  start
//...............................................................................

void LOGGING::start() {
  Serial.begin(115200);
  info("logging started");
}

//...............................................................................
//  setLogFunction()
//...............................................................................

void LOGGING::setLogFunction(const LogFunction lf) { logFunction = lf; }

//...............................................................................
//  log
//...............................................................................
void LOGGING::log(const String &channel, const String &msg) {
  if ((channel != "DEBUG") || DEBUG) {
    char txt[1024];
    String T = SysUtils::strDateTime(clock.now());
    sprintf(txt, "%s %5s %s", T.c_str(), channel.c_str(), msg.c_str());
    Serial.println(txt);
    if (logFunction != nullptr)
      logFunction(channel, msg);
  }
}

//...............................................................................
//  INFO
//...............................................................................
void LOGGING::info(const String &msg) { log(" INFO", msg); }

//...............................................................................
//  DEBUG
//...............................................................................
void LOGGING::debug(const String &msg) { log("DEBUG", msg); }

//...............................................................................
//  ERROR
//...............................................................................
void LOGGING::error(const String &msg) { log("ERROR", msg); }
