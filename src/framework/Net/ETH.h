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

class ETH {

private:
  Wiznet5500lwIP eth;

  LOGGING &logging;
  FFS &ffs;

  int present= 0;

public:
  ETH(LOGGING &logging, FFS &ffs);


  void start();
  void stop();
  void handle();
  int isPresent();
};