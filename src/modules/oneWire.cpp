#include "oneWire.h"

//===============================================================================
//  1 Wire
//===============================================================================
OW::OW(string name, LOGGING &logging, TopicQueue &topicQueue, int owPin, FFS &ffs)
      :Module(name, logging, topicQueue),
       owPin(owPin), oneWire(owPin), DS18B20(&oneWire), ffs(ffs)
       {}

//-------------------------------------------------------------------------------
//  1 Wire public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void OW::start() {
  Module::start();
/*
  logging.info("starting 1 Wire");
  scanBus();
  if (count == 0)
    logging.error("no 1 Wire devices found!!");
  else
  logging.info("1 Wire scan done, found " + String(count) + " devices");
  count = 0; //reset for first real loop run*/
}

//...............................................................................
// handle
//...............................................................................
void OW::handle() {
  Module::handle();
  unsigned long now = millis();

//poll measurement values
  if (now - tPoll > owPoll){
    tPoll = now;
    readDS18B20();
  }
}

//...............................................................................
// getVersion
//...............................................................................
String OW::getVersion() {
  return  String(OW_Name) + " v" + String(OW_Version);
}

//...............................................................................
//  scan OW-Bus for devices
//...............................................................................
void OW::scanBus() {
  DS18B20.begin();
  delay(100);
  DS18B20.setResolution(12);  //##
  delay(100);

  countOld = count;
  count = DS18B20.getDeviceCount();
  delay(100);
  DS18B20.requestTemperatures();
  delay(100);
  (countOld != count) ? (changed = true) : (changed = false);
}

//...............................................................................
//  read DS18B20
//...............................................................................
void OW::readDS18B20() {
  DynamicJsonBuffer root;
  //JsonObject& sensoren = root.parseObject(sensorenJson);
  JsonObject& sensoren = root.createObject();

  scanBus();

  String eventPrefix= "~/event/device/" + String(name) + "/";
  String strAddr = "";
  for (int i = 0; i < count; i++) {
    DS18B20.getAddress(addr, i);
    delay(100);

    //create sddress string
    strAddr = "";
    char str[5];
    for (int j = 0; j < 8; j++) {
      sprintf(str, "%02X", addr[j]);
      strAddr += str;
      if (j < 7) strAddr += "-";
    }

    //measure temperature
    String temp = String(DS18B20.getTempCByIndex(i));
    DS18B20.requestTemperatures();


    //publish
    //use alias name from ffs is exists
    String alias = ffs.deviceCFG.readItem(strAddr);
    if (alias == "") alias = strAddr;
    topicQueue.put(eventPrefix + "/" + alias + " " + temp);

    //assemble json
    sensoren[strAddr] = "";  //add item
  }

  if (changed) {
    sensorenJson = "";
    sensoren.printTo(sensorenJson);
    //sensoren.prettyPrintTo(Serial);
    topicQueue.put("~/event/device/sensorsChanged " + sensorenJson);
  }
}
