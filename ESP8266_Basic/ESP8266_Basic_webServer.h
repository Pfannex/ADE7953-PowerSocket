/******************************************************************************

  ProjectName: ESP8266 Basic                      ***** *****
  SubTitle   : WEBserver                         *     *     ************
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
#pragma once

  #include <Arduino.h>
  #include <ESP8266WiFi.h>			//https://github.com/esp8266/Arduino	

  #include <ESP8266WebServer.h>
  #include <WiFiClient.h> 
  #include <DNSServer.h>
  #include <ESP8266HTTPUpdateServer.h>

  #include <ESP8266_Basic_data.h>
  #include <functional>
  typedef std::function<void(void)> CallbackFunction;
  
  #define GPIO2 2

class ESP8266_Basic_webServer{
public:
  ESP8266_Basic_webServer();
  void start();
  void handleClient();
  
  void set_cfgPointer(CFG *p);  
  void set_saveConfig_Callback(CallbackFunction c);
    
private:
  ESP8266WebServer webServer;
  ESP8266HTTPUpdateServer httpUpdater;
  CallbackFunction saveConfig_Callback;
  CFG *cfg;
  
  //Page controls----------------
  void rootPageHandler();
  void wlanPageHandler();
  void gpioPageHandler();
  void cfgPageHandler();
  void handleNotFound();

  //helpers----------------------------
  String IPtoString(IPAddress ip);
};

