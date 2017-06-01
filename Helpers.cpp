#include "Helpers.h"

//###############################################################################
//  helpers 
//###############################################################################
Helpers::Helpers(){
  
}

//-------------------------------------------------------------------------------
//  helpers public
//-------------------------------------------------------------------------------
//...............................................................................
//  check FlashConfiguration
//...............................................................................
void Helpers::checkFlash(){
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
//  convert char to IP
//...............................................................................
IPAddress Helpers::charToIP(char* IP) {
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
IPAddress Helpers::strToIP(String IP) {
  IPAddress MyIP;
  
  MyIP = charToIP(string2char(IP.c_str()));
  return MyIP;
}
//...............................................................................
//  xxx
//...............................................................................
char* Helpers::string2char(String command){
  if(command.length()!=0){
    char *p = const_cast<char*>(command.c_str());
    return p;
  }
}


//-------------------------------------------------------------------------------
//  helpers private
//-------------------------------------------------------------------------------




