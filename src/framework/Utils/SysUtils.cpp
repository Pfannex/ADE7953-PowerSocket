#include "Setup.h"
#include "framework/Utils/SysUtils.h"

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
  char txt[11];
  sprintf(txt, "%02d.%02d.%04d", day(t), month(t), year(t));
  return String(txt);
}

String SysUtils::strDate_ms(unsigned long long t) { return strDate(t / 1000); }

String SysUtils::strTime(time_t t) {
  char txt[11];
  sprintf(txt, "%02d:%02d:%02d", hour(t), minute(t), second(t));
  return String(txt);
}

String SysUtils::strTimeHHMM(time_t t) {
  char txt[6];
  sprintf(txt, "%02d:%02d", hour(t), minute(t));
  return String(txt);
}

String SysUtils::strTime_ms(unsigned long long t) {
  char txt[128];
  time_t td = t / 1000;
  sprintf(txt, "%02d:%02d:%02d.%03ld", hour(td), minute(td), second(td),
          (unsigned int)(t - 1000 * td));
  return String(txt);
}

String SysUtils::strDateTime(time_t t) { return strDate(t) + " " + strTime(t); }

String SysUtils::strDateTime_ms(unsigned long long t) {
  return strDate_ms(t) + " " + strTime_ms(t);
}

String SysUtils::uptimeStr(unsigned long long t) {

  unsigned long td= t/1000;
  unsigned int ms= t-td*1000;
  long int days = td / 86400;
  td -= days * 86400;
  int hours = td / 3600;
  td -= hours * 3600;
  int minutes = td / 60;
  td -= minutes * 60;
  char txt[32];
  sprintf(txt, "%ldd %02d:%02d:%02d.%03ld", days, hours, minutes, td, ms);
  return String(txt);
}

//###############################################################################
//  version utils
//###############################################################################

String SysUtils::fullVersion() {
  return String(VERSION) + " " + String(__DATE__) + " " + String(__TIME__);
}
