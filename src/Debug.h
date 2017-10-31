#pragma once

#include <ESP.h>

//###############################################################################
//  macros for development
//###############################################################################

// print a message
#define D(msg) Serial.printf("  >>> %s (%d %s)\n", msg, __LINE__, __FILE__);

// print the current code line
#define Dl D("mark")

// print a message and an integer value
#define Di(msg, value)                                                         \
  Serial.printf("  >>> %s %d (%d %s)\n", msg, (value), __LINE__, __FILE__);

// print the free heap
#define DF(msg) Serial.printf("  >>> %s free heap= %d\n", msg, ESP.getFreeHeap());
