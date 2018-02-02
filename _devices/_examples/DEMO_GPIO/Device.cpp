#include "Device.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
Device::Device(LOGGING &logging, TopicQueue &topicQueue)
               : logging(logging), topicQueue(topicQueue),
               GPIOinput_01(IN01, logging, topicQueue),
               GPIOinput_02(IN02, logging, topicQueue),
               GPIOoutput_01(OUT01, logging, topicQueue),
               GPIOoutput_02(OUT02, logging, topicQueue) {}

//...............................................................................
// device start
//...............................................................................
void Device::start() {

  logging.info("starting Device " + String(DEVICETYPE) + " V" + String(DEVICEVERSION));

  //device start instructions
  GPIOinput_01.start();
  GPIOinput_02.start();
  GPIOoutput_01.start();
  GPIOoutput_02.start();

  logging.info("Device running");

}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void Device::handle() {
  GPIOinput_01.handle();
  GPIOinput_02.handle();
  GPIOoutput_01.handle();
  GPIOoutput_02.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String Device::set(Topic &topic) {
  /*
  ~/set                (Itemlevel 1)
    └─device           (Itemlevel 2)
      └─gpio           (Itemlevel 3)
  */

  logging.debug("Device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

//e.g.
  if (topic.itemIs(3, "gpio")) {
    GPIOinput_01.set(topic);
    GPIOinput_02.set(topic);
    GPIOoutput_01.set(topic);
    GPIOoutput_02.set(topic);
    return TOPIC_OK;
  }else{
    return TOPIC_NO;
  }
}
//...............................................................................
//  Device get
//...............................................................................

String Device::get(Topic &topic) {
  /*
  ~/get                (Itemlevel 2)
    └─function         (Itemlevel 3)
        └─sub function (Itemlevel 4)
  */

  logging.debug("Device get topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

//e.g.
  if (topic.itemIs(3, "gpio")) {
    GPIOinput_01.get(topic);
    GPIOinput_02.get(topic);
    GPIOoutput_01.get(topic);
    GPIOoutput_02.get(topic);
    return "get answer";
  }else{
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void Device::on_events(Topic &topic) {
  if (topic.itemIs(1, "event")){
    if (topic.itemIs(2, "gpio")){
      if (topic.itemIs(4, "click")){
        //check last state!
        int state = topic.getArgAsLong(0);
        if (state){
          //handle über set get
          //gpio.setOutputMode(OUT01, gpio.ON, 0);
          //gpio.setOutputMode(OUT01, gpio.OFF, 0);
        }
      }
    }
  }

/*
  String IO = topic.getItem(4);

  if (topicStr == "~/event/gpio/" + IO + "/state"){
    int io = IO.toInt();
    int state = topic.getArgAsLong(0);
    digitalWrite(io, state);

  }
*/


  GPIOinput_01.on_events(topic);

}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
