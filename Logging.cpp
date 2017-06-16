#include "Logging.h"
#include "SysUtils.h"

//###############################################################################
//  logging
//###############################################################################

void log(const String &channel, const String &msg) {

  char txt[1024];
  long ms= millis();
  long d= ms/86400000;
  ms-= d*86400000;
  long h= ms/3600000;
  ms-= h*3600000;
  long m= ms/60000;
  ms-= m*60000;
  long s= ms/1000;
  ms-= s*1000;
  
  sprintf(txt, "%4d:%02d:%02d:%02d.%03d %5s %s", d, h, m, s, ms, 
          channel.c_str(), msg.c_str());
  Serial.println(txt);
}

void info(const String &msg) {
  log("INFO ", msg);
}

void debug(const String &msg) {
  log("DEBUG", msg);
}

void error(const String &msg) {
  log("ERROR", msg);
}

void debugMem() {
  char msg[30];
  sprintf(msg, "free memory: %d", freeHeapSize());
  debug(msg);
}
