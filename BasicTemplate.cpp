#include "BasicTemplate.h"

//###############################################################################
//  BasicTemplate
//###############################################################################
BasicTemplate::BasicTemplate():
    ffs(i2c),
    mqtt(ffs, i2c, wifi),
    wifi(ffs, i2c), 
    webif(ffs) {  

//callback Events
  //WiFi
  wifi.set_callbacks(std::bind(&BasicTemplate::on_wifiConnected, this), 
                     std::bind(&BasicTemplate::on_x, this));
}

//-------------------------------------------------------------------------------
//  BasicTemplate public
//-------------------------------------------------------------------------------
//...............................................................................
//  Start WiFi Connection
//...............................................................................
void BasicTemplate::startConnections(){
  wifi.start();
}

//...............................................................................
//  handle connection
//...............................................................................
void BasicTemplate::handle(){
  timerUpdate();
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
//  Start Periphery
//...............................................................................
void BasicTemplate::startPeriphery(){
  i2c.start();
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void BasicTemplate::on_wifiConnected(){
  delay(1000);
  mqtt.start();
  webif.start();
 
}
//...............................................................................
//  EVENT wifi x
//...............................................................................
void BasicTemplate::on_x(){
  //Serial.println("on_x");  
}

//-------------------------------------------------------------------------------
//  BasicTemplate private
//-------------------------------------------------------------------------------
//...............................................................................
//  idle timer
//...............................................................................
void BasicTemplate::timerUpdate(){
  long now = millis();
  if (now - timerLastUpdate > timerUpdateTime){
    timerLastUpdate = now;

    Serial.println("Hello World");
  }
}






