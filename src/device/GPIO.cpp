#include "GPIO.h"
#include <Arduino.h>

//===============================================================================
//  GPIO
//===============================================================================

//-------------------------------------------------------------------------------
//  GPIO public
//-------------------------------------------------------------------------------
GPIO::GPIO(LOGGING &logging, TopicQueue &topicQueue)
    : logging(logging), topicQueue(topicQueue) {}

void GPIO::start() {

  logging.info("starting GPIO");

}

//...............................................................................
// handle
//...............................................................................

void GPIO::handle() {

}

//...............................................................................
//  GPIO set
//...............................................................................

String GPIO::set(Topic &topic) {
  /*
  ~/set                (Itemlevel 2)
    └─function         (Itemlevel 3)
        └─sub function (Itemlevel 4)
  */

  logging.debug("Device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

//e.g.
  if (topic.itemIs(3, "led")) {
    if (topic.itemIs(4, "01")){
      ///## your Event
      return TOPIC_OK;
    }else if (topic.itemIs(4, "02")){
      //## your Event
      return TOPIC_OK;
    }else{
      return TOPIC_NO;
    }
  }else{
    return TOPIC_NO;
  }
}

//...............................................................................
//  GPIO set
//...............................................................................

String GPIO::get(Topic &topic) {
  /*
  ~/get                (Itemlevel 2)
    └─function         (Itemlevel 3)
        └─sub function (Itemlevel 4)
  */

logging.debug("GPIO get topic " + topic.topic_asString() + " to " +
              topic.arg_asString());

  //e.g.
  if (topic.itemIs(3, "led")) {
    return "get answer";
  }else{
    return TOPIC_NO;
  }

}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------
