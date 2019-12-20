#pragma once

#include "Setup.h"
#include "framework/Core/FFS.h"
#include "framework/Utils/Logger.h"
#include <Arduino.h>
#include <W5500lwIP.h>
#include <SPI.h>

//###############################################################################
//  Ethernet
//###############################################################################
#define MAXTRIES 10 //tries to get IP by DHCP

class ETH {

private:
  Wiznet5500lwIP eth;

  LOGGING &logging;
  FFS &ffs;

  int present= 0;
  int plugged = 0;

public:
  ETH(LOGGING &logging, FFS &ffs);

  void start();
  void stop();
  void handle();
  int isPresent();
  int isPluggedIn();
};