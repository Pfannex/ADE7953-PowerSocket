#pragma once
#include <Arduino.h>
/*
extern "C" {
#include <cont.h>
extern cont_t g_cont;
}
*/
#include "framework/Utils/Logger.h"

//###############################################################################
//  ESP
//###############################################################################
class ESP_Tools {
public:
  ESP_Tools(LOGGING& logging);

  LOGGING& logging;

  void start();
  void checkFlash();
  uint32_t freeHeapSize();

  /*
  This portion was removed after g_cont was not available any more in a
  recent version of the Arduino library. As a positive side effect, untraceable
  hangs in the web interface disappeared.

  int freeStackSize();
  int minFreeStackSize();
  int stackCorrupted();

  void debugMem();
  void debugMem_start();
  void debugMem_stop();

  */

  void reboot();
  long chipId();
  String genericName();

  bool updateRequested();
  String update();

  String set(Topic &topic);
  String get(Topic &topic);

private:
  int stackStart = 0;
  char deviceName[10];
  bool updateRequest= false;
  bool setDeviceDefaults= false;

};
