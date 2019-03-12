#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  Device public
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs){//,
    //WS_DI("WS2812", logging, topicQueue, WS_PIN, LEDCOUNT) {

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

  //WS_DI.start();
  //String col = ffs.deviceCFG.readItem("drawer1_COLOR");
  //WS_DI.color = (int) strtol( &col[1], NULL, 16);


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



  //ws.begin();
  //ws.clear();



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
  String ret = TOPIC_NO;

//store all drawer/color
  if (topic.itemIs(3, "drawer")) {
    if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      ffs.deviceCFG.writeItem("drawer_COLOR", str);
      for (size_t i = 0; i < CHANNELSCOUNT; i++) {
        String strItem = "drawer" + String(i+1);
        if (ffs.deviceCFG.readItem(strItem + "_USECASE") == "drawer"){
          if (!str.startsWith("#")) str = "#" + str;
          ffs.deviceCFG.writeItem(strItem + "_COLOR", str);
        }
      }
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
  }

  if (topic.itemIs(3, "floor")) {
//store all floor/color
    if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      ffs.deviceCFG.writeItem("floor_COLOR", str);
      for (size_t i = 0; i < CHANNELSCOUNT; i++) {
        String strItem = "drawer" + String(i+1);
        if (ffs.deviceCFG.readItem(strItem + "_USECASE") == "floor"){
          if (!str.startsWith("#")) str = "#" + str;
          ffs.deviceCFG.writeItem(strItem + "_COLOR", str);
        }
      }
      ffs.deviceCFG.saveFile();
      return TOPIC_OK;
    }
//switch all floor
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
          //setStrip(i+1, topic.getArgAsLong(0));
        }
      }
      return TOPIC_OK;
    }
  }

  for (size_t i = 0; i < CHANNELSCOUNT; i++) {
    char ch[80] = "drawer";
    strcat (ch, String(i+1).c_str());
//switch individual drawer/floorlight
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




/*

  if (topic.itemIs(3, "drawer1")){
    if (topic.itemIs(4, "state")) {
      setChannel(0);
      if (topic.getArgAsLong(0)){
        Serial.println("on");
        String col = ffs.deviceCFG.readItem("drawer1_COLOR");
        //color = (int) strtol( &col[1], NULL, 16);
        setStrip(col);
      }else{
        Serial.println("off");
        setStrip(0);
      }
      return TOPIC_OK;
    }else if (topic.itemIs(4, "color")) {
      String str = topic.getArg(0);
      //color = (int) strtol( &str[1], NULL, 16);
      ffs.deviceCFG.writeItem("drawer1_COLOR", str);
      ffs.deviceCFG.saveFile();
      setChannel(1);
      setStrip(str);
      return TOPIC_OK;
    }else{
      return TOPIC_NO;
    }

  }else if (topic.itemIs(3, "drawer2")){
    return TOPIC_NO;

  }else{
    return TOPIC_NO;
  }
*/

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


  topicQueue.put("~/event/device/drawer/color " + ffs.deviceCFG.readItem("drawer_COLOR"));
  topicQueue.put("~/event/device/floor/color " + ffs.deviceCFG.readItem("floor_COLOR"));

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
  //delay(100);
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
/*
  setStrip(0, 0x00FF00);
  setStrip(1, 0x00FF00);
  setStrip(2, 0x00FF00);
  delay(500);

  setStrip(0, 0xFF0000);
  setStrip(1, 0x0000FF);
  setStrip(2, 0xFF00FF);
  delay(500);
*/


  for (byte i = 0; i < CHANNELSCOUNT; i++) {
    //select channel
    setChannel(i);
    //Serial.println("Channel: " + String(i));

    //select settings from ffs
    String strItem = "drawer" + String(i+1);
    //String col = ffs.deviceCFG.readItem(strItem + "_COLOR");
    String name = ffs.deviceCFG.readItem(strItem + "_NAME");
    int isDrawer = (ffs.deviceCFG.readItem(strItem + "_USECASE") == "drawer") ? (1) : (0);
    //color = (int) strtol( &col[1], NULL, 16);

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
      //Serial.println("Channel " + String(i) + " = " + String(diff));

      //check QRE-value
      unsigned long now = millis();

      //debouncing and state detection
      if (now - lastChangeTime[i] > DEBOUNCETIME){
        lastChangeTime[i] = now;

        //String IO = String(i);
        //Serial.println(QRE_Value);
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


/*
  for (byte i = 0; i < CHANNELSCOUNT; i++) {
    //select channel
    setChannel(i);
    //Serial.println("Channel: " + String(i));

    //select color from ffs
    if (i == 0) WS_DI.color = 0x0000FF;
    if (i == 1) WS_DI.color = 0x00FF00;
    if (i == 2) WS_DI.color = 0xFF0000;
    if (i == 0) WS_DI.count = 17;
    if (i == 1) WS_DI.count = 17;
    if (i == 2) WS_DI.count = 38;

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
  */
}
