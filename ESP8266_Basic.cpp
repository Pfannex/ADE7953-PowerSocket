#include "ESP8266_Basic.h"

//CLASS ESP8266_Basic################################################################
ESP8266_Basic::ESP8266_Basic():
    ffs(i2c){  
  
  // : webServer(), 
                                 //mqtt_client(wifi_client){


								 
  //Callbacks								 
  //webServer.set_saveConfig_Callback(std::bind(&ESP8266_Basic::cfgChange_Callback, this));
  //mqtt_client.setCallback(std::bind(&ESP8266_Basic::mqttBroker_Callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));


}

//===============================================================================
//  AktSen Control 
//===============================================================================

//===> Start WiFi Connection <-------------------------------------------------
void ESP8266_Basic::startConnections(){
  wifi.start();
}
//===> Start Periphery <-------------------------------------------------------
void ESP8266_Basic::startPeriphery(){
  i2c.start();
}

//===> updateMeasurement <-----------------------------------------------------
void ESP8266_Basic::TimerUpdate(){
  long now = millis();
  if (now - timerLastUpdate > timerUpdateTime){
    timerLastUpdate = now;

    Serial.println("Hello World");
      
  //if (mqtt_client.connected()){
      //run_oneWire();
      //run_I2C();
  //}
  }
}

//===> Check Flash Memory <-----------------------------------------------------
void ESP8266_Basic::checkFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  Serial.println("");
  Serial.println("============================================");
    Serial.printf("Flash real id:   %08X", ESP.getFlashChipId());
    Serial.println("");
    Serial.printf("Flash real size: %u", realSize);
    Serial.println("");

    Serial.printf("Flash ide  size: %u", ideSize);
    Serial.println("");
    Serial.printf("Flash ide speed: %u", ESP.getFlashChipSpeed());
    Serial.println("");
    Serial.printf("Flash ide mode:  %s", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
    Serial.println("");
    
    if(ideSize != realSize) {
        Serial.println("Flash Chip configuration wrong!");
    } else {
        Serial.println("Flash Chip configuration ok.");
    }
  Serial.println("============================================");
  Serial.println("");
}






