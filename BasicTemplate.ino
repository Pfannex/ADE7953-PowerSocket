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

#include "BasicTemplate.h"
BasicTemplate bt;


void setup() {  
  Serial.begin(115200); 
  Serial.println("");
  
  bt.checkFlash();  
  bt.startPeriphery();
  bt.ffs.mount();  
  bt.startConnections();

  
  //bt.ffs.TEST();
  
}  

void loop() {
  //bt.i2c.lcd.clear();
  //bt.ffs.i2c.lcd.println("Hello World!", 2);
  
  //bt.handle_connections(); 
  //bt.TimerUpdate();
  
}



