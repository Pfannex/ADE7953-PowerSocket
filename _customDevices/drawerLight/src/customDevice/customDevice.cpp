#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs){

  type= String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {

  Device::start();

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

  FastLED.addLeds<SM16703, WS_PIN, BRG>(leds, LEDCOUNT);
  for (int i = 0; i<CHANNELSCOUNT; i++){
    setChannel(i);
    setStrip(0);
  }

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
  └─device               (level 2)
    └─drawer             (level 3)
      └─color            (level 4)
      └─index            (level 4) individual setting
      └─name             (level 4) individual setting
      └─individualColor  (level 4) individual setting
      └─usecase          (level 4) individual setting

    └─floor            (level 3)
      └─color          (level 4)
      └─state          (level 4)
    └─drawerx          (level 3)  x=[1-16] drawer & floor
      └─state          (level 4)
      └─color          (level 4)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());
  String ret = TOPIC_NO;

  //DRAWER---------------------------
  if (topic.itemIs(3, "drawer")) {
    //store all drawer/color
    if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      if (!str.startsWith("#")) str = "#" + str;
      ffs.deviceCFG.writeItem("drawer_COLOR", str);
      for (size_t i = 0; i < CHANNELSCOUNT; i++) {
        String strItem = "drawer" + String(i+1);
        if (ffs.deviceCFG.readItem(strItem + "_USECASE") == "drawer"){
          ffs.deviceCFG.writeItem(strItem + "_COLOR", str);
        }
      }
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
    //individual settings
    //index-------------------
    if (topic.itemIs(4, "index")) {
      String strIndex = topic.getArg(0);
      if (strIndex == "next"){
        (index < CHANNELSCOUNT) ? (index++) : (index = 1);
        topicQueue.put("~/event/device/drawer/index " + String(index));
      }else if (strIndex == "previous"){
        (index > 1) ? (index--) : (index = CHANNELSCOUNT);
        topicQueue.put("~/event/device/drawer/index " + String(index));
      }else{
        index = topic.getArgAsLong(0);
      }

      String strItem = "drawer" + String(index);
      name = ffs.deviceCFG.readItem(strItem + "_NAME");
      color = ffs.deviceCFG.readItem(strItem + "_COLOR");
      usecase = ffs.deviceCFG.readItem(strItem + "_USECASE");
      topicQueue.put("~/event/device/drawer/name " + name);
      topicQueue.put("~/event/device/drawer/individualColor " + color);
      String uc = (usecase == "drawer") ? ("0") : ("1");
      topicQueue.put("~/event/device/drawer/usecase " + uc);
      return TOPIC_OK;
    }
    //name-------------------
    if (topic.itemIs(4, "name")) {
      String str = topic.getArg(0);
      String strItem = "drawer" + String(index);
      ffs.deviceCFG.writeItem(strItem + "_NAME", str);
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
    //individualColor-------------------
    if (topic.itemIs(4, "individualColor")) {
      String str = topic.getArg(0);
      String strItem = "drawer" + String(index);
      ffs.deviceCFG.writeItem(strItem + "_COLOR", str);
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
    //usecase--------------------
    if (topic.itemIs(4, "usecase")) {
      String str = topic.getArg(0);
      String strItem = "drawer" + String(index);
      ffs.deviceCFG.writeItem(strItem + "_USECASE", str);
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
  }

  //FLOOR---------------------------
  if (topic.itemIs(3, "floor")) {
    //store all floor/color-------------------
    if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      if (!str.startsWith("#")) str = "#" + str;
      ffs.deviceCFG.writeItem("floor_COLOR", str);
      for (size_t i = 0; i < CHANNELSCOUNT; i++) {
        String strItem = "drawer" + String(i+1);
        if (ffs.deviceCFG.readItem(strItem + "_USECASE") == "floor"){
          ffs.deviceCFG.writeItem(strItem + "_COLOR", str);
        }
      }
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
    //switch all floor-------------------
    if (topic.itemIs(4, "state")) {
      for (size_t i = 0; i < CHANNELSCOUNT; i++) {
        setChannel(i);
        String strItem = "drawer" + String(i+1);
        if (ffs.deviceCFG.readItem(strItem + "_USECASE") == "floor"){
          if (topic.getArgAsLong(0)){
            setStrip(i+1, 1);
          }else{
            setStrip(0);
          }
        }
      }
      return TOPIC_OK;
    }
  }

  //DRAWER[i]----------------------------------
  for (size_t i = 0; i < CHANNELSCOUNT; i++) {
    char ch[80] = "drawer";
    strcat (ch, String(i+1).c_str());
    //switch individual drawer/floorlight------
    if (topic.itemIs(3, ch)){
      if (topic.itemIs(4, "state")){
        setChannel(i);
        if (topic.getArgAsLong(0)){
          setStrip(i+1, 1);
        }else{
          setStrip(0);
        }
        return TOPIC_OK;
      }
      //set individual drawer/floorlight color
      if (topic.itemIs(4, "color")){
        setChannel(i);
        String str = topic.getArg(0);
        if (!str.startsWith("#")) str = "#" + str;
        ffs.deviceCFG.writeItem("drawer" + String(i+1) + "_COLOR", str);
        ffs.deviceCFG.saveFile();
        return TOPIC_OK;
      }
    }
  }
  return ret;
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
  topicQueue.put("~/event/device/drawer/color " + ffs.deviceCFG.readItem("drawer_COLOR"));
  topicQueue.put("~/event/device/floor/color " + ffs.deviceCFG.readItem("floor_COLOR"));

  topicQueue.put("~/event/device/drawer/index 1");
  topicQueue.put("~/set/device/drawer/index 1");

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
//  Stripcontroll
//...............................................................................
void customDevice::setStrip(int color){
  for (int i = 0; i<LEDCOUNT; i++){
    leds[i] = color;
  }
  FastLED.show();
}

void customDevice::setStrip(String col){
  if (col.length() == 7) col.remove(0,1);  //maybe a leading #
  if (col.length() == 6){
    int color = (int) strtol( &col[0], NULL, 16);
    setStrip(color);
  }else{
    setStrip(0);
  }
}

void customDevice::setStrip(int channel, int state){
  setChannel(channel-1);
  String strItem = "drawer" + String(channel);
  String col = ffs.deviceCFG.readItem(strItem + "_COLOR");
  setStrip(col);
}

//...............................................................................
//  handle Channels
//...............................................................................
void customDevice::handleChannels(){

  for (byte i = 0; i < CHANNELSCOUNT; i++) {
    //select channel
    setChannel(i);

    //select settings from ffs
    String strItem = "drawer" + String(i+1);
    String name = ffs.deviceCFG.readItem(strItem + "_NAME");
    int isDrawer = (ffs.deviceCFG.readItem(strItem + "_USECASE") == "drawer") ? (1) : (0);

    if (isDrawer){
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

      //check QRE-value
      unsigned long now = millis();

      //debouncing and state detection
      if (now - lastChangeTime[i] > DEBOUNCETIME){
        lastChangeTime[i] = now;

        String eventPrefix= "~/event/device/drawer/" + name + "/";

        if (diff > THRESHOLD_ON && !pinState[i]){
          pinState[i] = 1;
          logging.debug(name + " open");
          topicQueue.put(eventPrefix + "/state 1");
          setStrip(i+1, 1);
        }else if (diff < THRESHOLD_OFF && pinState[i]){
          pinState[i] = 0;
          logging.debug(name + " closed");
          topicQueue.put(eventPrefix + "/state 0");
          setStrip(0);
        }
      }
    }
  }
}
