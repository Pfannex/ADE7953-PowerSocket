#pragma once
  #include <Arduino.h>
  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino  

/*
//###############################################################################
//  ESP
//###############################################################################
class ESP{
public:
  Helpers();
  void checkFlash();
  IPAddress charToIP(char* IP);   
  IPAddress strToIP(String IP);   
  char* string2char(String command);
};*/


//###############################################################################
//  SysUtils
//###############################################################################
uint32_t freeHeapSize();
void reboot();
long chipID();
String macAddress();
