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

#include "TemplateController.h"
TemplateController tc;


void setup() {  
  Serial.begin(115200); 
  Serial.println("");
  
  tc.sysUtils.esp_tools.checkFlash();  
  tc.startPeriphery();
  tc.ffs.mount();  
  tc.startConnections();

  
  //tc.ffs.TEST();
  
}  

void loop() {
  tc.handle();
  
  //tc.i2c.lcd.clear();
  //tc.ffs.i2c.lcd.println("Hello World!", 2);
  
  //tc.handle_connections(); 
  //tc.TimerUpdate();
  
}



