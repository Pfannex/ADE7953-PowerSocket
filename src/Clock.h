#pragma once
#include "Setup.h"
#include "Topic.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

//###############################################################################
//  NTP clock
//###############################################################################

class Clock {

public:
  Clock(TopicQueue &topicQueue);
  ~Clock();
  void start();
  // offset in seconds, updateInterval in milliseconds
  void start(const char* poolServerName, int timeOffset, int updateInterval);
  void stop();
  void handle();
  void forceUpdate();
  time_t now();   // current time in seconds since the epoch

  String set(Topic& topic);
  String get(Topic& topic);
  String root();  // all data in JSON format


private:
  TopicQueue &topicQueue;
  WiFiUDP ntpUDP;
  NTPClient *ntpClient = nullptr;
  void adjustTimeOffset();
  unsigned long lastTime= 0; // lastTime in seconds to detect clock tick

};
