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
#include <SoftTimer.h>	//https://github.com/prampec/arduino-softtimer
						            //need https://github.com/prampec/arduino-pcimanager

#include "API.h"
API api;

//Timer
void Loop(Task* me);
void t_1s(Task* me);
void t_short(Task* me);
void t_long(Task* me);
Task t1(0, Loop);
Task t2(1000, t_1s);
Task t3(5000, t_short);
Task t4(3600000, t_long);

//-------------------------------------------------------------------------------
//  Setup
//-------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("");

  //Timer
  SoftTimer.add(&t1);
  SoftTimer.add(&t2);
  SoftTimer.add(&t3);
  SoftTimer.add(&t4);

  api.start();

  //api.api.set("/Hello/World/foo/bar", "arg1,arg2,3,4,5");
  //api.api.set("/Hello/World/foo/bar arg1,arg2,3,4,5");
}

//-------------------------------------------------------------------------------
//  timer control
//-------------------------------------------------------------------------------
void Loop(Task* me) {
  api.handle();
}
void t_1s(Task* me) {
  api.t_1s_Update();
}
void t_short(Task* me) {
  api.t_short_Update();
}
void t_long(Task* me) {
  api.t_long_Update();
}
