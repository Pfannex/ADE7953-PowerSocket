#include "TemplateController.h"

//###############################################################################
//  TemplateController
//###############################################################################
TemplateController::TemplateController():
    api(sysUtils, ffs),
    ffs(sysUtils, i2c),
    mqtt(sysUtils, api, ffs, i2c, wifi),
    wifi(sysUtils, api, ffs, i2c),
    webif(sysUtils, api){

//callback Events
  //WiFi
  wifi.set_callbacks(std::bind(&TemplateController::on_wifiConnected, this),
                     std::bind(&TemplateController::on_x, this));
}

//-------------------------------------------------------------------------------
//  start
//-------------------------------------------------------------------------------
void TemplateController::start(){
  sysUtils.clock.update(false);

  sysUtils.esp_tools.checkFlash();
  startPeriphery();
  ffs.mount();
  startConnections();
}

//...............................................................................
//  handle connection
//...............................................................................
void TemplateController::handle(){
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
void TemplateController::on_wifiConnected(){
  sysUtils.clock.start();
  sysUtils.clock.update(true);
  delay(200);
  mqtt.start();
  webif.start();
}

//...............................................................................
//  EVENT wifi x
//...............................................................................
void TemplateController::on_x(){
  //Serial.println("on_x");
}

//...............................................................................
//  idle timer
//...............................................................................
void TemplateController::t_1s_Update(){
  sysUtils.clock.update(false);  //t++
}
void TemplateController::t_short_Update(){
  sysUtils.logging.debugMem();
}
void TemplateController::t_long_Update(){
  sysUtils.clock.update(true);   //by NTP
}

//-------------------------------------------------------------------------------
//  TemplateController private
//-------------------------------------------------------------------------------
//...............................................................................
//  Start WiFi Connection
//...............................................................................
void TemplateController::startConnections(){
  wifi.start();
}

//...............................................................................
//  Start Periphery
//...............................................................................
void TemplateController::startPeriphery(){
  i2c.start();
}
