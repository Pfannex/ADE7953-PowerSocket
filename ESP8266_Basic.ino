/******************************************************************************

  ProjectName: ESP8266-1W-I2C-Gateway             ***** *****
  SubTitle   : 1W/I²C -> MQTT bridge             *     *     ************
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

  espClient.ffs.TEST();
  
  espClient.startConnections();
}  

void loop() {
  //espClient.handle_connections(); 
  espClient.TimerUpdate();
}



