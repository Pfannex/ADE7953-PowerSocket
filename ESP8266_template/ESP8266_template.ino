/******************************************************************************

  ProjectName: ESP8266 Basic                      ***** *****
  SubTitle   : Basic template                    *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 04.03.2016                            *****      *   *
  Version : alpha 0.10                                      *   *
  Revison :                                                *****

********************************************************************************/

#include <ESP8266_Basic.h>
ESP8266_Basic espClient;
 
void setup() {
  Serial.begin(115200); 
  Serial.println("");
  
  espClient.start_WiFi_connections();
}  

void loop() {
  espClient.handle_connections(); 
}



