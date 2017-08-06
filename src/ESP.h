#pragma once
#include <Arduino.h>
extern "C" {
#include <cont.h>
extern cont_t g_cont;
}
#include "Logger.h"

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
  int freeStackSize();
  int minFreeStackSize();
  int stackCorrupted();
  void reboot();
  long chipId();
  String genericName();

  void debugMem();
  void debugMem_start();
  void debugMem_stop();

  String set(Topic &topic);
  String get(Topic &topic);


private:
  int stackStart = 0;
  char deviceName[10];

};
