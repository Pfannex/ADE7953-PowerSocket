#include "device/_examples/DEMO_GPIO.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
DEMO_GPIO::DEMO_GPIO(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :device(logging, topicQueue, ffs),
            button("Walter", logging, topicQueue),
            mcp(logging, topicQueue){}

//...............................................................................
// device start
//...............................................................................
void DEMO_GPIO::start() {
  device::start();
  logging.info("child starting " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  button.start();
  mcp.start();

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void DEMO_GPIO::handle() {
  //button.handle();
  mcp.handle();
}


//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
