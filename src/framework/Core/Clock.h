#pragma once
#include "Setup.h"
#include "framework/Topic.h"
#include <NTPClient.h>
#include <Timezone.h>
#include <WiFiUdp.h>

//###############################################################################
//  NTP clock
//###############################################################################

class Clock {

public:
  Clock(TopicQueue &topicQueue);
  ~Clock();
  void start();
  // offset in seconds, updateInterval in milliseconds
  void start(const char *poolServerName, int timeOffset, int updateInterval);
  void stop();
  void handle();
  void forceUpdate();
  time_t now();    // current time in seconds since the epoch
  double uptime(); // uptime in s (with ms accuracy)

  String set(Topic &topic);
  String get(Topic &topic);
  String root(); // all data in JSON format

private:
  TopicQueue &topicQueue;
  WiFiUDP ntpUDP;
  NTPClient *ntpClient = nullptr;
  void updateUptime();
  unsigned long lastTime = 0; // lastTime in seconds to detect clock tick
  unsigned long uptimeLo = 0; // in ms
  unsigned long uptimeHi = 0; // in 4294967296 ms

  // time zone

  #ifdef TZ_CET
  // Central European Time (Frankfurt, Paris)
  // Central European Summer Time
  TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};
  // Central European Standard Time
  TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};
  #endif

  Timezone tz;
};
