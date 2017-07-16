#include "API.h"

//###############################################################################
//  API
//###############################################################################
API::API(SysUtlils& sysUtils,
         FFS& ffs,
         WiFi& wifi,
         MQTT& mqtt,
         WebIF& webif,
         I2C& i2c,
         OWIRE& oWire):

         sysUtils(sysUtils),
         ffs(ffs),
         wifi(wifi),
         mqtt(mqtt),
         webif(webif),
         i2c(i2c),
         oWire(oWire){

//callback Events
  //WiFi
  wifi.set_callbacks(std::bind(&API::on_wifiConnected, this),
                     std::bind(&API::on_x, this));
}

//-------------------------------------------------------------------------------
//  start
//-------------------------------------------------------------------------------
void API::start(){
  sysUtils.clock.update(false);

  sysUtils.esp_tools.checkFlash();
  startPeriphery();
  ffs.mount();
  startConnections();
}

//...............................................................................
//  handle connection
//...............................................................................
void API::handle(){
  if (wifi.handle()){
    if (!mqtt.handle()){
      Serial.println("MQTT has disconnected!");
      delay(1000);
      mqtt.start();
    }
    webif.handle();
  }else{
    Serial.println("WiFi has disconnected!");
    wifi.start();
  }
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void API::on_wifiConnected(){
  sysUtils.clock.start();
  sysUtils.clock.update(true);
  delay(200);
  mqtt.start();
  webif.start();
}

//...............................................................................
//  EVENT wifi x
//...............................................................................
void API::on_x(){
  //Serial.println("on_x");
}

//...............................................................................
//  idle timer
//...............................................................................
void API::t_1s_Update(){
  sysUtils.clock.update(false);  //t++
}
void API::t_short_Update(){
  sysUtils.logging.debugMem();
}
void API::t_long_Update(){
  sysUtils.clock.update(true);   //by NTP
}

//...............................................................................
//  API call
//...............................................................................
String API::call(Topic& topic){
  String ret = "NIL";

//set
  if (topic.itemIs(1, "set")){
    if (topic.itemIs(2, "ffs")){
      return ffs.set(topic);
    }else if (topic.itemIs(2, "sysUtils")) {
      return sysUtils.set(topic);
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.set(topic);
    }else{
      return "NIL";
    }
//get
  }else if (topic.itemIs(1, "get")){
    if (topic.itemIs(2, "ffs")){
      return ffs.get(topic);
    }else if (topic.itemIs(2, "sysUtils")) {
      return sysUtils.get(topic);
    }else if (topic.itemIs(2, "mqtt")) {
      //return mqtt.get(topic);
    }else{
      return "NIL";
    }
  }else{
    return "NIL";
  }
}

//-------------------------------------------------------------------------------
//  API private
//-------------------------------------------------------------------------------
//...............................................................................
//  Start WiFi Connection
//...............................................................................
void API::startConnections(){
  wifi.start();
}

//...............................................................................
//  Start Periphery
//...............................................................................
void API::startPeriphery(){
  i2c.start();
}
