#include "SysUtils.h"

//###############################################################################
//  SysUtils
//###############################################################################
SysUtils::SysUtils(){
}

//###############################################################################
//  NET WiFi/LAN
//###############################################################################
//-------------------------------------------------------------------------------
//  NET public
//-------------------------------------------------------------------------------
NET::NET(){
}

//...............................................................................
//  convert char to IP
//...............................................................................
IPAddress NET::charToIP(char* IP) {
  IPAddress MyIP;

  String str = String(IP);
  for (int i = 0; i < 4; i++){
    String x = str.substring(0, str.indexOf("."));
    MyIP[i] = x.toInt();
    str.remove(0, str.indexOf(".")+1); 
  }
  return MyIP;
}
//...............................................................................
//  convert String to IP
//...............................................................................
IPAddress NET::strToIP(String IP) {
  IPAddress MyIP;
  
  MyIP = charToIP(string2char(IP.c_str()));
  return MyIP;
}
//...............................................................................
//  conver String to Char
//...............................................................................
char* NET::string2char(String command){
  if(command.length()!=0){
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}

//...............................................................................
//  get MACAddress
//...............................................................................
String NET::macAddress() {
    uint8_t mac[6];
    char maddr[18];
    WiFi.macAddress(mac);
      sprintf(maddr, "%02x:%02x:%02x:%02x:%02x:%02x", 
                mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return String(maddr);
}


//###############################################################################
//  ESP
//###############################################################################
ESP_Tools::ESP_Tools(){
}

//-------------------------------------------------------------------------------
//  ESP public
//-------------------------------------------------------------------------------
//...............................................................................
//  check FlashConfiguration
//...............................................................................
void ESP_Tools::checkFlash(){
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  Serial.println("");
  Serial.println("............................................");
    Serial.printf("Flash real id:   %08X", ESP.getFlashChipId());
    Serial.println("");
    Serial.printf("Flash real size: %u", realSize);
    Serial.println("");

    Serial.printf("Flash ide  size: %u", ideSize);
    Serial.println("");
    Serial.printf("Flash ide speed: %u", ESP.getFlashChipSpeed());
    Serial.println("");
    Serial.printf("Flash ide mode:  %s", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
    Serial.println("");
    
    if(ideSize != realSize) {
        Serial.println("Flash Chip configuration wrong!");
    } else {
        Serial.println("Flash Chip configuration ok.");
    }
  Serial.println("............................................");
}

//...............................................................................
//  ESP reboot
//...............................................................................
void ESP_Tools::reboot() {
  // due to an issue in the silicon chip,
  // this works only after one hardware reset
  // has occured after flashing
  ESP.restart();
}

//...............................................................................
//  get free HeapSize
//...............................................................................
uint32_t ESP_Tools::freeHeapSize() {  //long?
  return ESP.getFreeHeap();
}

//...............................................................................
//  get Chip ID
//...............................................................................
long ESP_Tools::chipID() {
  return ESP.getChipId();
}

//###############################################################################
//  logging
//###############################################################################
LOGGING::LOGGING(){
}

//-------------------------------------------------------------------------------
//  LOGGING public
//-------------------------------------------------------------------------------
//...............................................................................
//  log
//...............................................................................
void LOGGING::log(const String &channel, const String &msg) {
  char txt[1024];
  long ms= millis();
  long d= ms/86400000;
  ms-= d*86400000;
  long h= ms/3600000;
  ms-= h*3600000;
  long m= ms/60000;
  ms-= m*60000;
  long s= ms/1000;
  ms-= s*1000;
  
  sprintf(txt, "%4d:%02d:%02d:%02d.%03d %5s %s", d, h, m, s, ms, 
          channel.c_str(), msg.c_str());
  Serial.println(txt);
}

//...............................................................................
//  INFO
//...............................................................................
void LOGGING::info(const String &msg) {
  log("INFO ", msg);
}

//...............................................................................
//  DEBUG
//...............................................................................
void LOGGING::debug(const String &msg) {
  log("DEBUG", msg);
}

//...............................................................................
//  ERROR
//...............................................................................
void LOGGING::error(const String &msg) {
  log("ERROR", msg);
}

//...............................................................................
//  DEBUG MEM
//...............................................................................
void LOGGING::debugMem() {
  char msg[30];
  sprintf(msg, "free memory: %d", esp_tools.freeHeapSize());
  debug(msg);
}

