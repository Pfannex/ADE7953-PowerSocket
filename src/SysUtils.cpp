#include "SysUtils.h"

// we do not need an instance of SysUtils
// all methods are static (class functions)

//###############################################################################
//  SysUtils
//###############################################################################

long SysUtils::seed = 0;

//...............................................................................
//  random
//...............................................................................

long SysUtils::rand(long max) {

  // We initialize the random number generator when it is first used.
  // Thus a random number of milliseconds has passed when we come here.
  // seed is a class variable (only one copy for all instances of Session
  if (!SysUtils::seed) {
    SysUtils::seed = millis();
    randomSeed(SysUtils::seed);
    // logging.info("initialized random generator.");
  }
  return random(max);
};

//###############################################################################
//  net string utils
//###############################################################################

//...............................................................................
//  convert char to IP
//...............................................................................
IPAddress SysUtils::charToIP(char *IP) {
  IPAddress MyIP;

  String str = String(IP);
  for (int i = 0; i < 4; i++) {
    String x = str.substring(0, str.indexOf("."));
    MyIP[i] = x.toInt();
    str.remove(0, str.indexOf(".") + 1);
  }
  return MyIP;
}
//...............................................................................
//  convert String to IP
//...............................................................................
IPAddress SysUtils::strToIP(String IP) {
  IPAddress MyIP;

  MyIP = charToIP(string2char(IP.c_str()));
  return MyIP;
}
//...............................................................................
//  conver String to Char
//...............................................................................
char *SysUtils::string2char(String command) {
  if (command.length() != 0) {
    char *p = const_cast<char *>(command.c_str());
    return p;
  }
}

//###############################################################################
//  date and time utils
//###############################################################################

String SysUtils::strDate(time_t t) {
  char txt[128];
  sprintf(txt, "%02d.%02d.%04d", day(t), month(t), year(t));
  return String(txt);
}

String SysUtils::strTime(time_t t) {
  char txt[128];
  sprintf(txt, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  return String(txt);
}

String SysUtils::strTime_ms(time_t t) {
  char txt[128];
  long ms = 1000 * (t - floor(t));
  sprintf(txt, "%02d:%02d:%02d.%03d", hour(t), minute(t), second(t), ms);
  return String(txt);
}

String SysUtils::strDateTime(time_t t) { return strDate(t) + " " + strTime(t); }

String SysUtils::strDateTime_ms(time_t t) {
  return strDate(t) + " " + strTime_ms(t);
}
