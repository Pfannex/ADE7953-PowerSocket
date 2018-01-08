/******************************************************************************

  ProjectName: OmniESP                            ***** *****
  SubTitle   : WLAN/LAN/MQTT/WebIf,LCD           *     *     ************
                                                *   **   **   *           *
                                               *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 13.11.2016                            *****      *   *
  Version : alpha 0.312                                     *   *
  Revison :                                                *****

********************************************************************************/
#include <SoftTimer.h> //https://github.com/prampec/arduino-softtimer
                       // need https://github.com/prampec/arduino-pcimanager

#include "API.h"
#include "Controller.h"
#include "WebServer.h"
#include "MQTT.h"
#include "Debug.h"

Controller controller;
API api(controller);
WebServer webServer(api);
MQTT mqtt(api);


// Timer
void Loop(Task *me);
void t_1s(Task *me);
void t_short(Task *me);
void t_long(Task *me);
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

  // we first start the controller
  // the controller initializes all subsystems in order
  controller.start();

  // we have the API as a level of abstraction
  api.start();

  // the viewers communicate with the susbsystems via the API
  //D("starting web interface");
  // TODO start only if we have WiFi or LAN!
  webServer.start();
  //D("starting MQTT");
  // TODO start only if we have WiFi or LAN!
  mqtt.start();

  // done
  api.info("startup finished");

  // Timer
  SoftTimer.add(&t1);
  SoftTimer.add(&t2);
  SoftTimer.add(&t3);
  SoftTimer.add(&t4);

  // controller.api.set("/Hello/World/foo/bar", "arg1,arg2,3,4,5");
  // controller.api.set("/Hello/World/foo/bar arg1,arg2,3,4,5");
}

//-------------------------------------------------------------------------------
//  timer control
//-------------------------------------------------------------------------------
void Loop(Task *me) {
  yield(); //  yield to allow ESP8266 background functions
  controller.handle();
  yield(); //  yield to allow ESP8266 background functions
  mqtt.handle();
}

void t_1s(Task *me) { controller.t_1s_Update(); }
void t_short(Task *me) { controller.t_short_Update(); }
void t_long(Task *me) { controller.t_long_Update(); }
