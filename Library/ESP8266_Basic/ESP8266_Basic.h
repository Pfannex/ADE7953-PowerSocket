/******************************************************************************

  ProjectName: ESP8266 Basic                      ***** *****
  SubTitle   : Basic Library                     *     *     ************
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
/*
ToDo

Reset nach nicht geänderter Config wird nicht durchgeführt
WEB-Status Tabelle rechts
TopicTree 
OTA WEB
OTA Arduino
OTA onDemand
WEBServer Seite in function mit übergabe Variablen und ''' 


*/  
  

#pragma once

//FileSystem
  #include <FS.h>                   	//this needs to be first, or it all crashes and burns...
  #include <ArduinoJson.h>          	//https://github.com/bblanchon/ArduinoJson
 
//ESP8266
  #include <ESP8266WiFi.h>			    //https://github.com/esp8266/Arduino	
  #include <EEPROM.h>

  #include <ESP8266WebServer.h>
  #include <WiFiClient.h> 
  #include <DNSServer.h>
  
//MQTT Client
  #include <PubSubClient.h>
  
//ESP8266_Basic
  #include <ESP8266_Basic_webServer.h>
  #include <ESP8266_Basic_data.h>
  
  
class ESP8266_Basic{

public:
  ESP8266_Basic();
  void start_WiFi_connections();
  void handle_connections();
  void cfgChange_Callback();
  void mqttBroker_callback(char* topic, byte* payload, unsigned int length);

private:
  WiFiClient wifi_client;
  PubSubClient mqtt_client;
  File cfgFile;
  ESP8266_Basic_webServer webServer;  
  
  //WiFi-Manager-Control---------------
  void startConfigServer();
  void startAccessPoint();
  
  bool start_WiFi();  
  bool start_MQTT();
  bool WIFI_reconnect;
  
  //Config-Control---------------------
  void read_cfg();
  bool read_cfgFile();
  void write_cfg();
  void write_cfgFile();
  void resetSettings();  
  bool config_running;
  CFG cfg;

  //MQTT-Control-----------------------
  Topics topic;
  String buildE1(int e1);
  String buildE2(int e1, int e2);
  String buildE3(int e1, int e2, int e3);
  //String buildE4(int e1, int e2, int e3, int e4);
  bool pub(int e1, char* Payload);
  bool pub(int e1, int e2, char* Payload);
  bool pub(int e1, int e2, int e3, char* Payload);
  //bool pub(int e1, int e2, int e3, int e4, char* Payload);
  TdissectResult dissectPayload(String subTopic, String subValue);
  
  void pubConfig();

  
  //Prüfen ob noch benötigt
  bool shouldSaveConfig;
  String TopicHeader = "999";
  
  //helpers----------------------------
  void checkFlash();
  String IPtoString(IPAddress ip);
  IPAddress charToIP(char* IP);
  void printCFG();    
};