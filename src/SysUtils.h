#pragma once
  #include <Arduino.h>
  extern "C" {
    #include <cont.h>
    extern cont_t g_cont;
  }
  #include "Setup.h"
  #include "Topic.h"
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  #include <WiFiUdp.h>
  #include <NTPClient.h>            //https://github.com/arduino-libraries/NTPClient
  #include <TimeLib.h>              //https://github.com/PaulStoffregen/Time

//###############################################################################
//  NET WiFi/LAN
//###############################################################################
class NET{
public:
  NET();
  IPAddress charToIP(char* IP);
  IPAddress strToIP(String IP);
  char* string2char(String command);
  String macAddress();
};

//###############################################################################
//  NTP clock
//###############################################################################
class Clock{
public:
  Clock();
  void start();
  void update(bool ntp); //true = NTP , false = t++
  void setClock();
  time_t t;

  int _day;
  int _month;
  int _year;
  int _dayOfWeek;
  int _hour;
  int _minute;
  int _second;
  long _milliSecond;
  String strDate;
  String strTime;
  String strTime_ms;
  String strDateTime;
  String strDateTime_ms;
  String root;

private:
  WiFiUDP ntpUDP;
  NTPClient ntpClient;
};

//###############################################################################
//  ESP
//###############################################################################
class ESP_Tools{
public:
  ESP_Tools();
  void checkFlash();
  uint32_t freeHeapSize();
  int freeStackSize();
  int minFreeStackSize();
  int stackCorrupted();
  void reboot();
  long chipID();
};

//###############################################################################
//  LOGGING
//###############################################################################
class LOGGING{
public:
  LOGGING(Clock& clock);
  Clock& clock;
  ESP_Tools esp_tools;

  int stackStart = 0;

  void log(const String &channel, const String &msg);

  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);
  void debugMem();
  void debugMem_start();
  void debugMem_stop();
};

//###############################################################################
//  SysUtils
//###############################################################################
class SysUtils{
public:
  SysUtils();
  NET net;
  Clock clock;
  ESP_Tools esp_tools;
  LOGGING logging;
  // random
  long rand(long max);
  static long seed; // class variable
//API
  bool set(String topic);
  String get(String topic);


};
