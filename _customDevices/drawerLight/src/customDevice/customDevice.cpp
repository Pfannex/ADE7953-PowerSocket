#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
    d1_IN("drawer1", logging, topicQueue, DRAWER1_IN),
    d1_OUT("drawer1", logging, topicQueue, DRAWER1_OUT, DRAWER1_LEDCOUNT),
    d2_IN("drawer2", logging, topicQueue, DRAWER2_IN),
    d2_OUT("drawer2", logging, topicQueue, DRAWER2_OUT, DRAWER2_LEDCOUNT) {

  type= String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {

  Device::start();
  /*
  logging.info("starting " +
               button.getVersion()); // only first time a class is started
  */

  d1_IN.start();
  d1_OUT.start();
  String col = ffs.deviceCFG.readItem("drawer1_COLOR");
  d1_OUT.color = (int) strtol( &col[1], NULL, 16);
  //------------------
  d2_IN.start();
  d2_OUT.start();
  col = ffs.deviceCFG.readItem("drawer2_COLOR");
  d2_OUT.color = (int) strtol( &col[1], NULL, 16);
  //------------------

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  d1_IN.handle();
  d1_OUT.handle();
  d2_IN.handle();
  d2_OUT.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String customDevice::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─drawer1          (level 3)
      └─color          (level 4)
      └─state          (level 4)
    └─drawer2          (level 3)
      └─color          (level 4)
      └─state          (level 4)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.itemIs(3, "drawer1")){
    if (topic.itemIs(4, "state")) {
      d1_OUT.WS2812_on(topic.getArgAsLong(0));
      return TOPIC_OK;
    }else if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      d1_OUT.color = (int) strtol( &str[1], NULL, 16);
      ffs.deviceCFG.writeItem("drawer1_COLOR", str);
      ffs.deviceCFG.saveFile();
      d1_OUT.WS2812_on(1);
      return TOPIC_OK;
    }else{
      return TOPIC_NO;
    }

  }else if (topic.itemIs(3, "drawer2")){

  }else{
    return TOPIC_NO;
  }

/*

  if (topic.itemIs(3, "color1")) {
    String str = topic.getArg(0);
    d1_OUT.color = (int) strtol( &str[1], NULL, 16);
    ffs.deviceCFG.writeItem("Drawer1_COLOR", str);
    ffs.deviceCFG.saveFile();
    d1_OUT.WS2812_on(1);
    return TOPIC_OK;
  } else if (topic.itemIs(3, "power1")) {
    d1_OUT.WS2812_on(topic.getArgAsLong(0));
    //setPowerMode(power ? 0 : 1);
    return TOPIC_OK;

  } else if (topic.itemIs(3, "color2")) {
    String str = topic.getArg(0);
    d2_OUT.color = (int) strtol( &str[1], NULL, 16);
    //ffs.deviceCFG.writeItem("WS2812_02_COLOR", String(d2_OUT.color));
    ffs.deviceCFG.writeItem("WS2812_02_COLOR", str);
    ffs.deviceCFG.saveFile();
    d2_OUT.WS2812_on(1);
    return TOPIC_OK;
  } else if (topic.itemIs(3, "power02")) {
    d2_OUT.WS2812_on(topic.getArgAsLong(0));
    //setPowerMode(power ? 0 : 1);
    return TOPIC_OK;




  } else if (topic.itemIs(3, "toggle")) {
    //setPowerMode(power ? 0 : 1);
    return TOPIC_OK;
  } else if (topic.itemIs(3, "led")) {
    if (topic.itemIs(4, "blink")) {
      //setLedMode(topic.getArgAsLong(0));
      return TOPIC_OK;
    }
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }*/
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //topicQueue.put("~/event/device/power", power);
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic){


  // listen to QRE ~/device/drawer1/state
  if (d1_IN.isForModule(topic)) {
    if (d1_IN.isItem(topic, "state"))
      d1_OUT.WS2812_on(topic.getArgAsLong(0));
  }
  // listen to ~/device/drawer2/state
  if (d2_IN.isForModule(topic)) {
    if (d2_IN.isItem(topic, "state"))
      d2_OUT.WS2812_on(topic.getArgAsLong(0));
  }


/*
  // listen to ~/device/led/setmode
  if (led.isForModule(topic)) {
    if (led.isItem(topic, "setmode"))
      setLedMode(topic.getArgAsLong(0));
  }

  // central business logic
  if (button.isForModule(topic)) {
    // events from button
    //
    // - click
    if (button.isItem(topic, "click")) {
      // -- short
      if (topic.argIs(0, "short")) {
        if (configMode)
          setConfigMode(0);
        else
          setPowerMode(!power);
      }
      // -- long
      if (topic.argIs(0, "long"))
        setConfigMode(!configMode);
    }
    // - idle
    if (button.isItem(topic, "idle"))
      setConfigMode(0);
  }
*/
}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................
String customDevice::fillDashboard() {
  topicQueue.put("~/event/device/drawer1/state", d1_IN.state());
  topicQueue.put("~/event/device/drawer1/color " + ffs.deviceCFG.readItem("drawer1_COLOR"));

  topicQueue.put("~/event/device/drawer2/state", d2_IN.state());
  topicQueue.put("~/event/device/drawer2/color " + ffs.deviceCFG.readItem("drawer2_COLOR"));

  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
//  mode setter
//...............................................................................
