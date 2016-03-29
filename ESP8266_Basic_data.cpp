/******************************************************************************

  ProjectName: ESP8266_Basic                      ***** *****
  SubTitle   : ESP8266 Template                  *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 29.03.2016                            *****      *   *
  Version : alpha 0.200                                     *   *
  Revison :                                                *****

********************************************************************************/
#include <Arduino.h>
#include "ESP8266_Basic_data.h"

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
   ¦  ¦-[03] MQTT
   ¦  ¦  ¦-[030] Server
   ¦  ¦  ¦-[031] Port
   ¦  ¦  +-[032] Status
   ¦  +-[04] UpdateServer
   ¦     ¦-[040] Server
   ¦     +-[041] FilePath
   ¦     
   ¦-[1] Control
   ¦  ¦-[10] ESP8266
   ¦  ¦  ¦-[100] Reboot
   ¦  ¦  +-[101] ShowConfig   
   ¦  ¦  +-[102] Update Firmware   
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
  sub.E2.item[0][4] = "UpdateServer";
  sub.E3.item[0][4][0] = "Server";
  sub.E3.item[0][4][1] = "FilePath";
  
  sub.E1.item[1] = "Control";
  sub.E2.item[1][0] = "ESP8266";
  sub.E3.item[1][0][0] = "Reboot";
  sub.E3.item[1][0][1] = "ShowConfig";
  sub.E3.item[1][0][2] = "updateFirmware";
  sub.E3.item[1][0][3] = "MeasureTime";

  sub.E1.item[3] = "File";
  sub.E2.item[3][0] = "Read";
  sub.E3.item[3][0][0] = "Field_01";
  sub.E3.item[3][0][1] = "Field_02";
  sub.E2.item[3][1] = "Write";
  sub.E3.item[3][1][0] = "Field_01";
  sub.E3.item[3][1][1] = "Field_02";

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
   ¦  ¦-[03] MQTT
   ¦  ¦  ¦-[030] Server
   ¦  ¦  ¦-[031] Port
   ¦  ¦  +-[032] Status
   ¦  +-[04] UpdateServer
   ¦     ¦-[040] Server
   ¦     +-[041] FilePath
   ¦     
   ¦-[1] Control
   ¦  +-[10] Status
   ¦     ¦-[100] WiFi
   ¦     ¦-[101] MQTT
   ¦     +-[101] update Firmware   
   ¦
   ¦-[2] Sensor
   ¦  ¦-[20] temperatur
   ¦  ¦  ¦-[200] T1
   ¦  ¦  +-[201] T2   
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
  pub.E2.item[0][4] = "UpdateServer";
  pub.E3.item[0][4][0] = "Server";
  pub.E3.item[0][4][1] = "FilePath";
  
  pub.E1.item[1] = "Control";
  pub.E2.item[1][0] = "Status";
  pub.E3.item[1][0][0] = "WiFi";
  pub.E3.item[1][0][1] = "MQTT";
  pub.E3.item[1][0][2] = "updateFirmware";

  pub.E1.item[2] = "Sensor";
  pub.E2.item[2][1] = "temperature";
  pub.E3.item[2][1][0] = "T1";
  pub.E3.item[2][1][1] = "T2";
  pub.E3.item[2][1][2] = "T3";
  pub.E3.item[2][1][3] = "T4";
  pub.E3.item[2][1][4] = "T5";
  pub.E3.item[2][1][5] = "T6";
  pub.E3.item[2][1][6] = "T7";
  pub.E3.item[2][1][7] = "T8";
  pub.E3.item[2][1][8] = "T9";
  pub.E3.item[2][1][9] = "T10";
  pub.E3.item[2][1][10] = "T11";
  pub.E3.item[2][1][11] = "T12";
  pub.E3.item[2][1][12] = "T13";
  pub.E3.item[2][1][13] = "T14";
  pub.E3.item[2][1][14] = "T15";
  pub.E3.item[2][1][15] = "T16";
  pub.E3.item[2][1][16] = "T17";
  pub.E3.item[2][1][17] = "T18";
  pub.E3.item[2][1][18] = "T19";
  pub.E3.item[2][1][19] = "T20";
  pub.E3.item[2][1][20] = "T21";
  pub.E3.item[2][1][21] = "T22";
  pub.E3.item[2][1][22] = "T23";
  pub.E3.item[2][1][23] = "T24";
  pub.E3.item[2][1][24] = "T25";
  pub.E3.item[2][1][25] = "T26";
  pub.E3.item[2][1][26] = "T27";
  pub.E3.item[2][1][27] = "T28";
  pub.E3.item[2][1][28] = "T29";
  pub.E3.item[2][1][29] = "T30";


}
