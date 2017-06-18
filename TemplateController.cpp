#include "TemplateController.h"

//###############################################################################
//  TemplateController
//###############################################################################
TemplateController::TemplateController():
    ffs(i2c),
    mqtt(ffs, i2c, wifi),
    wifi(ffs, i2c), 
    webif(ffs) {  

//callback Events
  //WiFi
  wifi.set_callbacks(std::bind(&TemplateController::on_wifiConnected, this), 
                     std::bind(&TemplateController::on_x, this));
}

//-------------------------------------------------------------------------------
//  TemplateController public
//-------------------------------------------------------------------------------
//...............................................................................
//  Start WiFi Connection
//...............................................................................
void TemplateController::startConnections(){
  wifi.start();
}

//...............................................................................
//  handle connection
//...............................................................................
void TemplateController::handle(){
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
void TemplateController::startPeriphery(){
  i2c.start();
}

//...............................................................................
//  EVENT Wifi has connected
//...............................................................................
void TemplateController::on_wifiConnected(){
  delay(1000);
  mqtt.start();
  webif.start();
 
}
//...............................................................................
//  EVENT wifi x
//...............................................................................
void TemplateController::on_x(){
  //Serial.println("on_x");  
}

//-------------------------------------------------------------------------------
//  TemplateController private
//-------------------------------------------------------------------------------
//...............................................................................
//  idle timer
//...............................................................................
void TemplateController::timerUpdate(){
  long now = millis();
  if (now - timerLastUpdate > timerUpdateTime){
    timerLastUpdate = now;

    //Serial.println("Hello World");
    sysUtils.logging.debugMem();
}
}






