/*
extern "C" {
  #include "user_interface.h"
}

uint32_t freeHeapSize() {
  return system_get_free_heap_size();
}
*/

#include <ESP8266WiFi.h>

void reboot() {
  // due to an issue in the silicon chip,
  // this works only after one hardware reset
  // has occured after flashing
  ESP.restart();
}

long freeHeapSize() {
  return ESP.getFreeHeap();
}

long chipID() {
  return ESP.getChipId();
}

String macAddress() {
  
    uint8_t mac[6];
    char maddr[18];
    WiFi.macAddress(mac);
      sprintf(maddr, "%02x:%02x:%02x:%02x:%02x:%02x", 
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return String(maddr);
}
