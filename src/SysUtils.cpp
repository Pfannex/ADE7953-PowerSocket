#include "SysUtils.h"

//###############################################################################
//  SysUtils
//###############################################################################
SysUtils::SysUtils():
          logging(clock){
}

long SysUtils::seed= 0;

//...............................................................................
//  random
//...............................................................................

long SysUtils::rand(long max) {

    // We initialize the random number generator when it is first used.
    // Thus a random number of milliseconds has passed when we come here.
    // seed is a class variable (only one copy for all instances of Session
    if(!SysUtils::seed) {
        SysUtils::seed= millis();
        randomSeed(SysUtils::seed);
        logging.info("initialized random generator.");
    }
    return random(max);
};

//-------------------------------------------------------------------------------
//  SysUtils API
//-------------------------------------------------------------------------------
//...............................................................................
//  SysUtils SET
//...............................................................................
/*
sysUtils
  ├──esp
  │   └─restart
  └─clock
      └─forceNTP
*/
String SysUtils::set(Topic& topic){
//esp
  if (topic.itemIs(3, "esp")){
    if (topic.itemIs(4, "restart")) {
      esp_tools.reboot();
    }
//clock
  }else if (topic.itemIs(3, "clock")) {
    if (topic.itemIs(4, "forceNTP")) {
      clock.update(true);
      return "OK";
    }
  }
}

//...............................................................................
//  SysUtils GET
//...............................................................................
/*
sysUtils
  ├──esp
  │   └─freeHeapSize
  └─clock
      ├─root
      ├─Time
      ├─Date
      ├─DateTime
      └─DateTime_ms
*/
String SysUtils::get(Topic& topic){
//esp
  if (topic.itemIs(3, "esp")){
    if (topic.itemIs(4, "freeHeapSize")){
      return String(esp_tools.freeHeapSize());
    }
//clock
  }else if (topic.itemIs(3, "clock")) {
    if (topic.itemIs(4, "root")) {
      return clock.root;
    }else if (topic.itemIs(4, "time")) {
      return clock.strTime;
    }else if (topic.itemIs(4, "date")) {
      return clock.strDate;
    }else if (topic.itemIs(4, "dateTime")) {
      return clock.strDateTime;
    }else if (topic.itemIs(4, "dateTime_ms")) {
      return clock.strDateTime_ms;
    }else{
      return "NIL";
    }
  }else{
    return "NIL";
  }
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
//  NTP clock
//###############################################################################
//-------------------------------------------------------------------------------
//  NTP clock public
//-------------------------------------------------------------------------------
Clock::Clock():
       ntpClient(ntpUDP, NTP_SERVER, SUMMER_TIME, 30000){
}

//...............................................................................
//  start ntp
//...............................................................................
void Clock::start(){
  ntpClient.begin();
  ntpClient.update();
}

//...............................................................................
//  update clock
//...............................................................................
void Clock::update(bool ntp){
Serial.println("Clock::update(bool ntp)");

  if (ntp){
    char txt[1024];
    sprintf(txt, "%s %5s %s", strDateTime_ms.c_str(), "INFO", "NTP update");
    Serial.println(txt);
    t = ntpClient.getEpochTime();
  }else{
    t++;
  }
  setClock();
}

//-------------------------------------------------------------------------------
//  NTP clock private
//-------------------------------------------------------------------------------
//...............................................................................
//  update clock
//...............................................................................
void Clock::setClock(){
  Serial.println("Clock::setClock()");
  long now = millis();

  //Sommerzeit = letzter Sonntag im März von 2h -> 3h
  if (month(t) == 3 and day(t) >= 25 and weekday(t) == 7 and hour(t) == 2) ntpClient.setTimeOffset(SUMMER_TIME);
  //Winterzeit = letzter Sonntag im Oktober von 3h -> 2h
  if (month(t) == 10 and day(t) >= 25 and weekday(t) == 7 and hour(t) == 3) ntpClient.setTimeOffset(WINTER_TIME);

  _day         = day(t);
  _month       = month(t);
  _year        = year(t);
  _dayOfWeek   = weekday(t);
  _hour        = hour(t);
  _minute      = minute(t);
  _second      = second(t);

  long tmp = 0;
  _milliSecond  = millis();
  tmp           = _milliSecond/86400000;
  _milliSecond -= tmp*86400000;
  tmp           = _milliSecond/3600000;
  _milliSecond -= tmp*3600000;
  tmp           = _milliSecond/60000;
  _milliSecond -= tmp*60000;
  tmp           = _milliSecond/1000;
  _milliSecond -= tmp*1000;

  char txt[1024];
  sprintf(txt, "%02d.%02d.%04d",
               _day, _month, _year);
  strDate = txt;
  sprintf(txt, "%02d:%02d:%02d",
               _hour, _minute, _second);
  strTime = txt;
  sprintf(txt, "%02d:%02d:%02d.%03d",
               _hour, _minute, _second, _milliSecond);
  strTime_ms = txt;
  strDateTime = strDate + " - " + strTime;
  strDateTime_ms = strDate + " - " + strTime_ms;

  //date|time|time_ms|dateTime|dateTime_ms

  root = "{\"date\":\"" + strDate + "\"," +
          "\"time\":\"" + strTime + "\"," +
          "\"time_ms\":\"" + strTime_ms + "\"," +
          "\"dateTime\":\"" + strDateTime + "\"," +
          "\"dateTime_ms\":\"" + strDateTime_ms + "\"}";

  Serial.println("Clock::setClock() END");

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
//  get free memSize
//...............................................................................
uint32_t ESP_Tools::freeHeapSize() {  //long?
  return ESP.getFreeHeap();
}

int ESP_Tools::freeStackSize() {
  register uint32_t *sp asm("a1");
  return 4*(sp - g_cont.stack);
}

int ESP_Tools::minFreeStackSize() {
  return cont_get_free_stack(&g_cont);
}

int ESP_Tools::stackCorrupted() {
  return cont_check(&g_cont);
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
LOGGING::LOGGING(Clock& clock):
         clock(clock){
}

//-------------------------------------------------------------------------------
//  LOGGING public
//-------------------------------------------------------------------------------
//...............................................................................
//  log
//...............................................................................
void LOGGING::log(const String &channel, const String &msg) {
  clock.setClock();
  char txt[1024];
  sprintf(txt, "%s %5s %s", clock.strDateTime_ms.c_str(), channel.c_str(), msg.c_str());
  Serial.println(txt);
}

//...............................................................................
//  INFO
//...............................................................................
void LOGGING::info(const String &msg) {
  log(" INFO", msg);
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
  char msg[200];
  sprintf(msg, "free heap: %d, free stack: %d (min: %d, corrupt: %d)",
    esp_tools.freeHeapSize(),
    esp_tools.freeStackSize(),
    esp_tools.minFreeStackSize(),
    esp_tools.stackCorrupted());
  debug(msg);
}
void LOGGING::debugMem_start(){
  stackStart = esp_tools.freeStackSize();
  debugMem();
}
void LOGGING::debugMem_stop() {
  debugMem();
  char msg[200];
  sprintf(msg, "StackStart: %d, StackStop: %d, diff %d",
    stackStart,
    esp_tools.freeStackSize(),
    esp_tools.freeStackSize()-stackStart);
    debug(msg);
}
