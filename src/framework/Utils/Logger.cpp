#include "framework/Utils/Logger.h"
#include "framework/Utils/SysUtils.h"

//###############################################################################
//  logging
//###############################################################################
LOGGING::LOGGING(Clock &clock) : clock(clock) { logFunction = nullptr; }

//-------------------------------------------------------------------------------
//  LOGGING public
//-------------------------------------------------------------------------------

//...............................................................................
//  start
//...............................................................................

void LOGGING::start() {
  setLogFunction(nullptr);
  info("logging started");
#ifdef DEBUG
  debug("log level is DEBUG");
#endif
}

//...............................................................................
//  setLogFunction()
//...............................................................................

void LOGGING::setLogFunction(const LogFunction lf) { logFunction = lf; }

//...............................................................................
//  log
//...............................................................................

#define LOGMAXMSGLEN (256)

void LOGGING::log(const String &channel, const String &msg) {
// Dl;
#ifndef DEBUG
  if (channel == "DEBUG")
    return; // early exit
#endif
#ifdef TIMESTAMP_MS
  char *txt = (char *)malloc((31 + msg.length()) * sizeof(char));
  String T = SysUtils::strDateTime_ms(clock.nowMillis());
  sprintf(txt, "%.215s %.5s %s", T.c_str(), channel.c_str(),
          msg.substring(0, LOGMAXMSGLEN-1).c_str());
#else
  char *txt = (char *)malloc((27 + msg.length()) * sizeof(char));
  String T = SysUtils::strDateTime(clock.now());
  sprintf(txt, "%.211s %.5s %s", T.c_str(), channel.c_str(),
          msg.substring(0, LOGMAXMSGLEN-1).c_str());
#endif
  Serial.println(txt);
  free(txt);
  // Dl;
  if (logFunction != nullptr) {
    // D("calling logFunction...");
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
