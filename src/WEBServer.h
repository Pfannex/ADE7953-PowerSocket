#pragma once
  #include <Arduino.h>
  #include <ESP8266WiFi.h>			//https://github.com/esp8266/Arduino

  #include <ESP8266WebServer.h>
  #include <WiFiClient.h>
  #include <DNSServer.h>
  #include <ESP8266HTTPUpdateServer.h>
  #include <ESP8266httpUpdate.h>

  //#include "API.h"
  #include "Auth.h"
  #include "SysUtils.h"
  #include "FS.h"
  #include "Hash.h"

  #include <functional>
  typedef std::function<void(void)> CallbackFunction;

//###############################################################################
//  web interface
//###############################################################################
// https://links2004.github.io/Arduino/index.html

class WEBIF {
public:
  WEBIF(SysUtils& sysUtils);
  void start();
  void handle();

private:
  ESP8266WebServer webServer;
  ESP8266HTTPUpdateServer httpUpdater;
  SysUtils& sysUtils;
  //API& api;

  // authenticator
  Auth auth;
  bool checkAuthentification();

  // number of pages served
  long numPagesServed= 0;

  // serve file with some logging
  void send(const String &description, int code, char *content_type, const String &content);

  // send a file
  void sendFile(const String &description, int code, char *content_type, const String filePath);

  // page handler
  void rootPageHandler();
  void authPageHandler();
  void apiPageHandler();
  void handleNotFound();

  // variable substitution
  String subst(String data);

  // configuration
  void applyConfiguration();
  String getConfiguration();

};
