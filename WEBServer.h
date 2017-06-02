#pragma once
  #include <Arduino.h>
  #include <ESP8266WiFi.h>			//https://github.com/esp8266/Arduino	

  #include <ESP8266WebServer.h>
  #include <WiFiClient.h> 
  #include <DNSServer.h>
  #include <ESP8266HTTPUpdateServer.h>
  #include <ESP8266httpUpdate.h>

  #include <functional>
  typedef std::function<void(void)> CallbackFunction;
  
//###############################################################################
//  WEB-Interface 
//###############################################################################
class WEBIF{
public:
  WEBIF();
  void start();
  void handle();
  
  //void set_cfgPointer(CFG *p);  
  //void set_sensorPointer(TDS18B20_Sensors *p, THTU21_Sensors *q, TBMP180_Sensors *r);  
  //void set_saveConfig_Callback(CallbackFunction c);
  //void updateFirmware();
  
private:
  ESP8266WebServer webServer;
  ESP8266HTTPUpdateServer httpUpdater;
  
  //CallbackFunction saveConfig_Callback;
  //CFG *cfg;
  //TDS18B20_Sensors *DS18B20_Sensors; 
  //THTU21_Sensors *HTU21_Sensors; 
  //TBMP180_Sensors *BMP180_Sensors; 
  
  //Page controls----------------
  void rootPageHandler();
  void testPageHandler();
  //void sensorPageHandler();

  //void wlanPageHandler();
  //void gpioPageHandler();
  //void cfgPageHandler();
  void handleNotFound();

  //helpers----------------------------
  //String IPtoString(IPAddress ip);
};

