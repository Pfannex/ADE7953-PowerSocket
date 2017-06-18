#pragma once
  #include <Arduino.h>
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino  

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
//  ESP
//###############################################################################
class ESP_Tools{
public:
  ESP_Tools();
  void checkFlash();
  uint32_t freeHeapSize();
  void reboot();
  long chipID();
};

//###############################################################################
//  LOGGING
//###############################################################################
class LOGGING{
public:
  LOGGING();
  ESP_Tools esp_tools;
  
  void log(const String &channel, const String &msg);
  
  void info(const String &msg);
  void error(const String &msg);
  void debug(const String &msg);
  void debugMem();
};

//###############################################################################
//  SysUtils
//###############################################################################
class SysUtils{
public:
  SysUtils();
  NET net;
  ESP_Tools esp_tools;
  LOGGING logging;
};
