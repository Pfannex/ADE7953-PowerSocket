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
    WS_DI("WS2812", logging, topicQueue, WS_PIN, LEDCOUNT) {

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

  WS_DI.start();
  String col = ffs.deviceCFG.readItem("drawer1_COLOR");
  WS_DI.color = (int) strtol( &col[1], NULL, 16);


  pinMode(QRE_PIN, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(QRE_PIN, LOW);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  logging.info("device running");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  handleChannels();
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
      WS_DI.WS2812_on(topic.getArgAsLong(0));
      return TOPIC_OK;
    }else if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      WS_DI.color = (int) strtol( &str[1], NULL, 16);
      ffs.deviceCFG.writeItem("drawer1_COLOR", str);
      ffs.deviceCFG.saveFile();
      WS_DI.WS2812_on(1);
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
}

//...............................................................................
//  on request, fillDashboard with values
//...............................................................................
String customDevice::fillDashboard() {
  //topicQueue.put("~/event/device/drawer1/state", QRE_IN.state());
  topicQueue.put("~/event/device/drawer1/color " + ffs.deviceCFG.readItem("drawer1_COLOR"));


  logging.debug("dashboard filled with values");
  return TOPIC_OK;
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
//  select Channel
//...............................................................................
void customDevice::setChannel(int channel){
  ((channel & 1) == 1) ? (digitalWrite(S0, HIGH)) : (digitalWrite(S0, LOW));
  ((channel & 2) == 2) ? (digitalWrite(S1, HIGH)) : (digitalWrite(S1, LOW));
  ((channel & 4) == 4) ? (digitalWrite(S2, HIGH)) : (digitalWrite(S2, LOW));
  ((channel & 8) == 8) ? (digitalWrite(S3, HIGH)) : (digitalWrite(S3, LOW));
}

//...............................................................................
//  handle Channels
//...............................................................................
void customDevice::handleChannels(){

  for (byte i = 0; i < CHANNELSCOUNT; i++) {
    //select channel
    setChannel(i);
    //Serial.println("Channel: " + String(i));

    //select color from ffs
    if (i == 0) WS_DI.color = 0x0000FF;
    if (i == 1) WS_DI.color = 0x00FF00;
    if (i == 2) WS_DI.color = 0xFF0000;

    //read QRE-value
    pinMode(QRE_PIN, OUTPUT);
    digitalWrite(QRE_PIN, HIGH);
    delayMicroseconds(10);
    pinMode(QRE_PIN, INPUT );

    long time = micros();
    //time how long the input is HIGH, but quit after 3ms as nothing happens after that
    int diff = 0;
    while (digitalRead(QRE_PIN) == HIGH and diff < TIMEOUT){
      diff = micros() - time;
    }
    //Serial.println("Channel " + String(i) + " = " + String(diff));

    //check QRE-value
    unsigned long now = millis();

    //debouncing and state detection
    if (now - lastChangeTime[i] > DEBOUNCETIME){
      lastChangeTime[i] = now;

      String IO = String(i);
      //Serial.println(QRE_Value);
      String eventPrefix= "~/event/device/drawer" + String(i) + "/";

      if (diff > THRESHOLD_ON && !pinState[i]){
        pinState[i] = 1;
        logging.debug("Drawer " + String(i) + " open");
        topicQueue.put(eventPrefix + "/state 1");
        WS_DI.WS2812_on(1);
      }else if (diff < THRESHOLD_OFF && pinState[i]){
        pinState[i] = 0;
        logging.debug("Drawer " + String(i) + " closed");
        topicQueue.put(eventPrefix + "/state 0");
        WS_DI.WS2812_on(0);
      }
    }
  }
}
