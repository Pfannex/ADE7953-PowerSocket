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
#include <ESP8266_Basic_webServer.h>

ESP8266_Basic_webServer::ESP8266_Basic_webServer() : webServer(80){ 

  httpUpdater.setup(&webServer); 
 
  webServer.on("/", std::bind(&ESP8266_Basic_webServer::rootPageHandler, this));
  webServer.on("/wlan_config", std::bind(&ESP8266_Basic_webServer::wlanPageHandler, this));
  webServer.on("/gpio", std::bind(&ESP8266_Basic_webServer::gpioPageHandler, this));
  //webServer.on("/cfg", std::bind(&ESP8266_Basic_webServer::cfgPageHandler, this));
  webServer.onNotFound(std::bind(&ESP8266_Basic_webServer::handleNotFound, this));
}

//===============================================================================
//  WEB-Server control
//===============================================================================
void ESP8266_Basic_webServer::start(){
  Serial.println("Start WEB-Server");
  
  pinMode(GPIO2, OUTPUT);
  webServer.begin(); 
  Serial.println("HTTP server started");

}
//===> handle WEB-Server <-----------------------------------------------------
void ESP8266_Basic_webServer::handleClient(){
   webServer.handleClient();
}
//===> CFGstruct pointer <-----------------------------------------------------
void ESP8266_Basic_webServer::set_cfgPointer(CFG *p){
  cfg = p;
}
//===> Callback for CFGchange <------------------------------------------------
void ESP8266_Basic_webServer::set_saveConfig_Callback(CallbackFunction c){
  saveConfig_Callback = c;
}


//===============================================================================
//  HTML handling
//===============================================================================

//#############################################################################
void ESP8266_Basic_webServer::rootPageHandler()
{
  // Check if there are any GET parameters
  if (webServer.hasArg("webUser")) strcpy(cfg->webUser, webServer.arg("webUser").c_str());
  if (webServer.hasArg("webPassword")) strcpy(cfg->webPassword, webServer.arg("webPassword").c_str());
  if (webServer.hasArg("apName")) strcpy(cfg->apName, webServer.arg("apName").c_str());
  if (webServer.hasArg("apPassword")) strcpy(cfg->apPassword, webServer.arg("apPassword").c_str());
  if (webServer.hasArg("wifiSSID")) strcpy(cfg->wifiSSID, webServer.arg("wifiSSID").c_str());
  if (webServer.hasArg("wifiPSK")) strcpy(cfg->wifiPSK, webServer.arg("wifiPSK").c_str());
  if (webServer.hasArg("wifiIP")) strcpy(cfg->wifiIP, webServer.arg("wifiIP").c_str());
  if (webServer.hasArg("mqttServer")) strcpy(cfg->mqttServer, webServer.arg("mqttServer").c_str());
  if (webServer.hasArg("mqttPort")) strcpy(cfg->mqttPort, webServer.arg("mqttPort").c_str());
  if (webServer.hasArg("mqttDeviceName")) strcpy(cfg->mqttDeviceName, webServer.arg("mqttDeviceName").c_str());
  

  String rm = ""
  
  "<!doctype html> <html>"
  "<head> <meta charset='utf-8'>"
  "<title>ESP8266 Configuration</title>"
  "</head>"

  "<body><body bgcolor='#F0F0F0'><font face='VERDANA,ARIAL,HELVETICA'> <form> <font face='VERDANA,ARIAL,HELVETICA'>"
  "<b><h1>ESP8266 Configuration</h1></b>";

  if (WiFi.status() == WL_CONNECTED){
    rm += "ESP8266 connected to: "; rm += WiFi.SSID(); rm += "<br>";
    rm += "DHCP IP: "; rm += String(IPtoString(WiFi.localIP())); rm += "<p></p>";
  }else{
    rm += "ESP8266 ist not connected!"; rm += "<p></p>";
  }	
  
  String str = String(cfg->mqttStatus);
  if (str == "connected"){
    rm += "ESP8266 connected to MQTT-Broker: "; rm += cfg->mqttServer; rm += "<p></p>";
  }

  rm += ""
  "<table width='30%' border='0' cellpadding='0' cellspacing='2'>"
  " <tr>"
  "  <td><b><font size='+1'>WEB Server</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Username</td>"
  "  <td><input type='text' id='webUser' name='webUser' value='" + String(cfg->webUser) + "' size='30' maxlength='40' placeholder='Username'></td>"
  " </tr>"
  " <tr>"
  " <tr>"
  "  <td>Password</td>"
  "  <td><input type='text' id='webPassword' name='webPassword' value='" + String(cfg->webPassword) + "' size='30' maxlength='40' placeholder='Password'></td>"
  " </tr>"
  " <tr>"

  " <tr>"
  "  <td><b><font size='+1'>Accesspoint</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>SSID</td>"
  "  <td><input type='text' id='apName' name='apName' value='" + String(cfg->apName) + "' size='30' maxlength='40' placeholder='SSID'></td>"
  " </tr>"
  " <tr>"
  " <tr>"
  "  <td>Password</td>"
  "  <td><input type='text' id='apPassword' name='apPassword' value='" + String(cfg->wifiSSID) + "' size='30' maxlength='40' placeholder='Password'></td>"
  " </tr>"
  " <tr>"

  " <tr>"
  "  <td><b><font size='+1'>WiFi</font></b></td>"
  "  <td></td>"
  " </tr>"
  "  <td>SSID</td>"
  "  <td><input type='text' id='wifiSSID' name='wifiSSID' value='" + String(cfg->wifiSSID) + "' size='30' maxlength='40' placeholder='SSID'></td>"
  " </tr>"
  " <tr>"
  " <tr>"
  "  <td>Password</td>"
  "  <td><input type='text' id='wifiPSK' name='wifiPSK' value='" + String(cfg->wifiPSK) + "' size='30' maxlength='40' placeholder='Password'></td>"
  " </tr>"
  " <tr>"

  " <tr>"
  "  <td><b><font size='+1'>MQTT</font></b></td>"
  "  <td></td>"
  " </tr>"
  " <tr>"
  "  <td>Broker IP</td>"
  "  <td><input type='text' id='mqttServer' name='mqttServer' value='" + String(cfg->mqttServer) + "' size='30' maxlength='40' placeholder='IP Address'></td>"
  " </tr>"
  " <tr>"
  " <tr>"
  "  <td>Port</td>"
  "  <td><input type='text' id='mqttPort' name='mqttPort' value='" + String(cfg->mqttPort) + "' size='30' maxlength='40' placeholder='Port'></td>"
  " </tr>"
  " <tr>"
  "  <td>Devicename</td>"
  "  <td><input type='text' id='mqttDeviceName' name='mqttDeviceName' value='" + String(cfg->mqttDeviceName) + "' size='30' maxlength='40' placeholder='Devicename'></td>"
  " </tr>"

  " <tr>"
  "  <td><p></p> </td>"
  "  <td>  </td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='submit' value='Configuration sichern' style='height:30px; width:200px' > </font></form> </td>"
  " </tr>"

  " <tr>"
  "  <td></td>"
  "  <td>  </td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='submit' value='RESET' id='reset' name='reset' value='' style='height:30px; width:200px' >  </font></form> </td>"
  " </tr>"

  " <tr>"
  "  <td></td>"
  "  <td>  </td>"
  " </tr>"
  " <tr>"
  "  <td></td>"
  "  <td><input type='button' onclick=\"location.href='./update'\"  value='Flash Firmware' style='height:30px; width:200px' >  </font></form> </td>"
  " </tr>"

  "</table>"

  "<font size='-2'>&copy; by Pf@nne/16</font>"
  "</body bgcolor> </body></font>"
  "</html>"  
  ;
  
  
  
  
/*
 
  rm += "<!doctype html> <html>";
  rm += "<head> <meta charset=\"utf-8\"><title>ESP8266 Configuration</title></head>";
  rm += "<body><body bgcolor=\"#F0F0F0\"><font face=\"VERDANA,ARIAL,HELVETICA\"> <form> <font face=\"VERDANA,ARIAL,HELVETICA\">";
  rm += "<b><h1>ESP8266 Configuration</h1></b>";
  if (WiFi.status() == WL_CONNECTED){
    rm += "ESP8266 connected to: "; rm += WiFi.SSID(); rm += "<br>";
    rm += "DHCP IP: "; rm += String(IPtoString(WiFi.localIP())); rm += "<p></p>";
  }else{
    rm += "ESP8266 ist not connected!"; rm += "<p></p>";
  }	
  
  String str = String(cfg->mqttStatus);
  if (str == "connected"){
    rm += "ESP8266 connected to MQTT-Broker: "; rm += cfg->mqttServer; rm += "<br>";
  }
  
  rm += "<table width=\"35%\" border=\"0\" cellpadding=\"0\" cellspacing=\"2\">";
  
  rm += " <tr><td> <b><h3> WEB Server </h3></b> </td></tr>";
  rm += " <tr><td> Username </td><td> <input type=\"text\" id=\"webUser\" name=\"webUser\" value=\"";
  rm += cfg->webUser;
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Username\"></td></tr>";
  rm += "<tr><td> Password </td><td> <input type=\"text\" id=\"webPassword\" name=\"webPassword\" value=\"";

  rm += cfg->webPassword;
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Password\"></td></tr>";
  
  rm += " <tr><td> <b><h3> Accesspoint </h3></b> </td><td>";
  rm += " <tr><td> SSID </td><td> <input type=\"text\" id=\"apName\" name=\"apName\" value=\"";
  rm += cfg->apName;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"SSID\"></td></tr>";
  rm += " <tr><td> Password </td><td> <input type=\"text\" id=\"apPassword\" name=\"apPassword\" value=\"";
  rm += cfg->apPassword;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Password\"></td></tr>";
  
  rm += " <tr><td> <b><h3> WiFi </h3></b></td></tr>";
  rm += " <tr><td> SSID </td><td> <input type=\"text\" id=\"wifiSSID\" name=\"wifiSSID\" value=\"";
  rm += cfg->wifiSSID;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"SSID\"></td></tr>";
  rm += " <tr><td> Password </td><td> <input type=\"text\" id=\"wifiPSK\" name=\"wifiPSK\" value=\"";
  rm += cfg->wifiPSK;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Password\"></td></tr>";
  
  rm += " <tr><td><b><h3>MQTT</h3></b></td>";
  rm += " <tr><td> Broker IP </td><td> <input type=\"text\" id=\"mqttServer\" name=\"mqttServer\" value=\"";
  rm += cfg->mqttServer;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"IP Address\"></td></tr>";
  rm += " <tr><td> Port </td><td><input type=\"text\" id=\"mqttPort\" name=\"mqttPort\" value=\"";
  rm += cfg->mqttPort;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Port\"></td></tr>";
  rm += " <tr><td> Devicename </td><td> <input type=\"text\" id=\"mqttDeviceName\" name=\"mqttDeviceName\" value=\"";
  rm += cfg->mqttDeviceName;  
  rm += "\" size=\"30\" maxlength=\"40\" placeholder=\"Devicename\"></td></tr>";
  
  rm += " <td><p></p></td><td></td></tr>";
  rm += " <tr><td></td><td> <input type=\"submit\" value=\"Configuration sichern\" style=\"height:30px; width:200px\" > </font></form> </td></tr>";

  rm += "<tr><td></td><td></td></tr>";
  rm += "<tr><td></td><td><input type=\"submit\" value=\"RESET\" id=\"reset\" name=\"reset\" value=\"\" style=\"height:30px; width:200px\" >  </font></form> </td></tr>";

  rm += "<tr><td></td><td></td></tr>";
  rm += "<tr><td></td><td><input type=\"button\" onclick=\"location.href='/update';\" value=\"Flash Firmware\" style=\"height:30px; width:200px\" >  </font></form> </td> </tr>";

  rm += "</table>";

  rm += " <p></p></body bgcolor></body></font></html>  ";  
  rm += "<font size=\"-2\">&copy; by Pf@nne/16</font>";
  
*/  

  webServer.send(200, "text/html", rm);
 
  if (saveConfig_Callback != nullptr)
    saveConfig_Callback();
  else
     Serial.println("null");
	 
  if (webServer.hasArg("reset")) ESP.restart();
}

//#############################################################################

void ESP8266_Basic_webServer::wlanPageHandler()
{
  // Check if there are any GET parameters
  if (webServer.hasArg("ssid"))
  {    
    if (webServer.hasArg("password"))
    {
      WiFi.begin(webServer.arg("ssid").c_str(), webServer.arg("password").c_str());
    }
    else
    {
      WiFi.begin(webServer.arg("ssid").c_str());
    }
    
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
      
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(1000);
  }
  
  String response_message = "";
  response_message += "<html>";
  response_message += "<head><title>ESP8266 Webserver</title></head>";
  response_message += "<body style=\"background-color:PaleGoldenRod\"><h1><center>WLAN Settings</center></h1>";
  
  if (WiFi.status() == WL_CONNECTED)
  {
    response_message += "Status: Connected<br>";
  }
  else
  {
    response_message += "Status: Disconnected<br>";
  }
  
  response_message += "<p>To connect to a WiFi network, please select a network...</p>";

  // Get number of visible access points
  int ap_count = WiFi.scanNetworks();
  
  if (ap_count == 0)
  {
    response_message += "No access points found.<br>";
  }
  else
  {
    response_message += "<form method=\"get\">";

    // Show access points
    for (uint8_t ap_idx = 0; ap_idx < ap_count; ap_idx++)
    {
      response_message += "<input type=\"radio\" name=\"ssid\" value=\"" + String(WiFi.SSID(ap_idx)) + "\">";
      response_message += String(WiFi.SSID(ap_idx)) + " (RSSI: " + WiFi.RSSI(ap_idx) +")";
      (WiFi.encryptionType(ap_idx) == ENC_TYPE_NONE) ? response_message += " " : response_message += "*";
      response_message += "<br><br>";
    }
    
    response_message += "WiFi password (if required):<br>";
    response_message += "<input type=\"text\" name=\"password\"><br>";
    response_message += "<input type=\"submit\" value=\"Connect\">";
    response_message += "</form>";
  }

  response_message += "</body></html>";
  
  webServer.send(200, "text/html", response_message);
}


void ESP8266_Basic_webServer::gpioPageHandler()
{
  // Check if there are any GET parameters
  if (webServer.hasArg("gpio2"))
  { 
    if (webServer.arg("gpio2") == "1")
    {
      digitalWrite(GPIO2, HIGH);
    }
    else
    {
      digitalWrite(GPIO2, LOW);
    }
  }

  String response_message = "<html><head><title>ESP8266 Webserver</title></head>";
  response_message += "<body style=\"background-color:PaleGoldenRod\"><h1><center>Control GPIO pins</center></h1>";
  response_message += "<form method=\"get\">";

  response_message += "GPIO2:<br>";

  if (digitalRead(GPIO2) == LOW)
  {
    response_message += "<input type=\"radio\" name=\"gpio2\" value=\"1\" onclick=\"submit();\">On<br>";
    response_message += "<input type=\"radio\" name=\"gpio2\" value=\"0\" onclick=\"submit();\" checked>Off<br>";
  }
  else
  {
    response_message += "<input type=\"radio\" name=\"gpio2\" value=\"1\" onclick=\"submit();\" checked>On<br>";
    response_message += "<input type=\"radio\" name=\"gpio2\" value=\"0\" onclick=\"submit();\">Off<br>";
  }

  response_message += "</form></body></html>";

  
  webServer.send(200, "text/html", response_message);
}


void ESP8266_Basic_webServer::handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  
  for (uint8_t i = 0; i < webServer.args(); i++)
  {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  
  webServer.send(404, "text/plain", message);
}

//===============================================================================
//  helpers
//===============================================================================

//===> IPToString  <-----------------------------------------------------------
String ESP8266_Basic_webServer::IPtoString(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}
