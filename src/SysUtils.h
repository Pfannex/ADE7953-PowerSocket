#pragma once
#include "TimeLib.h"
#include <Arduino.h>
#include <IPAddress.h>

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
  static String strTime(time_t t);
  static String strTime_ms(time_t t);
  static String strDateTime(time_t t);
  static String strDateTime_ms(time_t t);
};
