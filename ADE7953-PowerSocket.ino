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

//void callBack1(Task* me);
//void callBack2(Task* me);

//Task t1(0, callBack1);
//Task t2(2000, callBack2);


void setup() {  
  Serial.begin(115200); 
  Serial.println("");
  
  //SoftTimer.add(&t1);
  //SoftTimer.add(&t2);
  
  tc.sysUtils.esp_tools.checkFlash();  
  tc.startPeriphery();
  tc.ffs.mount();  
  tc.startConnections();

  
  //tc.api.set("/Hello/World/foo/bar", "arg1,arg2,3,4,5");
  //tc.api.set("/Hello/World/foo/bar arg1,arg2,3,4,5");
  
}  

void loop() {
  tc.handle();
  
  //tc.i2c.lcd.clear();
  //tc.ffs.i2c.lcd.println("Hello World!", 2);
  
  //tc.handle_connections(); 
  //tc.TimerUpdate();
  
}

/*
void callBack1(Task* me) {
  tc.handle();
}
void callBack2(Task* me) {
  Serial.println("Hello TASK");
}*/




