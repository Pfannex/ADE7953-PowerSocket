#pragma once
#include "TimeLib.h"
#include <Arduino.h>
#include <IPAddress.h>
#include <FS.h>

//###############################################################################
//  SysUtils
//###############################################################################

class SysUtils {
public:

  // random number generator
  static long rand(long max);
  static long seed; // class variable

  // net string routines
  static IPAddress charToIP(char *IP);
  static IPAddress strToIP(String IP);
  static char *string2char(String command);

  // date and time utils
  static String strDate(time_t t);
  static String strDate_ms(unsigned long long  t);
  static String strTime(time_t t);
  static String strTimeHHMM(time_t t);
  static String strTime_ms(unsigned long long  t);
  static String strDateTime(time_t t);
  static String strDateTime_ms(unsigned long long t);
  static String uptimeStr(unsigned long long t);

  // version utils
  static String fullVersion();

};
