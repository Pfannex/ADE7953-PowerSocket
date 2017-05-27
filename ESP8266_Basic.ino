/******************************************************************************

  ProjectName: Basic template                     ***** *****
  SubTitle   : WLAN/LAN/MQTT/WebIf,LCD           *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 13.11.2016                            *****      *   *
  Version : alpha 0.312                                     *   *
  Revison :                                                *****

********************************************************************************/

#include "ESP8266_Basic.h"
ESP8266_Basic espClient;


void setup() {  
  Serial.begin(115200); 
  Serial.println("");
  
  espClient.checkFlash();  
  espClient.startPeriphery();
  espClient.ffs.mount();
  
  //espClient.startConnections();
  
}  

void loop() {
  //espClient.i2c.lcd.clear();
  //espClient.ffs.i2c.lcd.println("Hello World!", 2);
  
  //espClient.handle_connections(); 
  //espClient.TimerUpdate();
  
}



