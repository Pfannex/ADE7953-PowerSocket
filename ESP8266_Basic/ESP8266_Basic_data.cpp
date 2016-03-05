/******************************************************************************

  ProjectName: ESP8266 Basic                      ***** *****
  SubTitle   : Data structs                      *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 04.03.2016                            *****      *   *
  Version : alpha 0.101                                     *   *
  Revison :                                                *****

********************************************************************************/
#include <Arduino.h>
#include <ESP8266_Basic_data.h>

Topics::Topics(){ 

/* MQTT subcribe TopicTree struct   !!define TreeDepht in .h!!!
  [x] mqttDeviceName 
   ¦
   ¦-[0] WiFiConfig
   ¦  ¦-[00] WEBserver
   ¦  ¦  ¦-[000] Username
   ¦  ¦  +-[001] Password
   ¦  ¦-[01] Accesspoint
   ¦  ¦  ¦-[010] SSID
   ¦  ¦  +-[011] Password
   ¦  ¦-[02] WiFi
   ¦  ¦  ¦-[020] SSID
   ¦  ¦  ¦-[021] Password
   ¦  ¦  +-[022] IP
   ¦  +-[03] MQTT
   ¦     ¦-[030] Server
   ¦     ¦-[031] Port
   ¦     +-[032] Status
   ¦     
   ¦-[1] Control
   ¦  ¦-[10] ESP8266
   ¦  ¦  ¦-[100] Reboot
   ¦  ¦  +-[101] ShowConfig   
*/

  sub.E1.count = sub_e1;
  sub.E2.count = sub_e2;
  sub.E3.count = sub_e3;
  
  sub.E1.item[0] = "WiFiConfig";
  sub.E2.item[0][0] = "WEBserver";
  sub.E3.item[0][0][0] = "Username";
  sub.E3.item[0][0][1] = "Password";
  sub.E2.item[0][1] = "Accesspoint";
  sub.E3.item[0][1][0] = "SSID";
  sub.E3.item[0][1][1] = "Password";
  sub.E2.item[0][2] = "WiFi";
  sub.E3.item[0][2][0] = "SSID";
  sub.E3.item[0][2][1] = "Password";
  sub.E3.item[0][2][2] = "IP";
  sub.E2.item[0][3] = "MQTT";
  sub.E3.item[0][3][0] = "Server";
  sub.E3.item[0][3][1] = "Port";
  sub.E3.item[0][3][2] = "Status";
  
  sub.E1.item[1] = "Control";
  sub.E2.item[1][0] = "ESP8266";
  sub.E3.item[1][0][0] = "Reboot";
  sub.E3.item[1][0][1] = "ShowConfig";


/* MQTT publish TopicTree struct   !!define TreeDepht in .h!!!
  [x] mqttDeviceName 
   ¦
   ¦-[0] WiFiConfig
   ¦  ¦-[00] WEBserver
   ¦  ¦  ¦-[000] Username
   ¦  ¦  +-[001] Password
   ¦  ¦-[01] Accesspoint
   ¦  ¦  ¦-[010] SSID
   ¦  ¦  +-[011] Password
   ¦  ¦-[02] WiFi
   ¦  ¦  ¦-[020] SSID
   ¦  ¦  ¦-[021] Password
   ¦  ¦  +-[022] IP
   ¦  +-[03] MQTT
   ¦     ¦-[030] Server
   ¦     ¦-[031] Port
   ¦     +-[032] Status
   ¦     
   ¦-[1] Control
   ¦  ¦-[10] Status
   ¦  ¦  ¦-[100] WiFi
   ¦  ¦  +-[101] MQTT   
*/

  pub.E1.count = pub_e1;
  pub.E2.count = pub_e2;
  pub.E3.count = pub_e3;
  
  pub.E1.item[0] = "WiFiConfig";
  pub.E2.item[0][0] = "WEBserver";
  pub.E3.item[0][0][0] = "Username";
  pub.E3.item[0][0][1] = "Password";
  pub.E2.item[0][1] = "Accesspoint";
  pub.E3.item[0][1][0] = "SSID";
  pub.E3.item[0][1][1] = "Password";
  pub.E2.item[0][2] = "WiFi";
  pub.E3.item[0][2][0] = "SSID";
  pub.E3.item[0][2][1] = "Password";
  pub.E3.item[0][2][2] = "IP";
  pub.E2.item[0][3] = "MQTT";
  pub.E3.item[0][3][0] = "Server";
  pub.E3.item[0][3][1] = "Port";
  pub.E3.item[0][3][2] = "Status";
  
  pub.E1.item[1] = "Control";
  pub.E2.item[1][0] = "Status";
  pub.E3.item[1][0][0] = "WiFi";
  pub.E3.item[1][0][1] = "MQTT";

}
