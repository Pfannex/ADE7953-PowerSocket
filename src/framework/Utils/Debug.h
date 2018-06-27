#pragma once
#include <Arduino.h>

// #include <ESP.h> no need to include this because here are only macros!

//###############################################################################
//  macros for development
//###############################################################################

// print a message
#define D(msg) Serial.printf("  >>> %s (%d %s)\r\n", msg, __LINE__, __FILE__);

// print the current code line
#define Dl D("mark")

// print a message and an integer value
#define Di(msg, value)                                                         \
  Serial.printf("  >>> %s %d (%d %s)\r\n", msg, (value), __LINE__, __FILE__);

// print a message and string value
#define Ds(msg, value)                                                         \
  Serial.printf("  >>> %s %s (%d %s)\r\n", msg, (value), __LINE__, __FILE__);

// print the free heap
#define DF(msg)                                                                \
  Serial.printf("  >>> %s free heap= %d\r\n", msg, ESP.getFreeHeap());
