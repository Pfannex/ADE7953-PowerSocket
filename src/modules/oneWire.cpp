#include "oneWire.h"

//===============================================================================
//  1 Wire
//===============================================================================
OW::OW(string name, LOGGING &logging, TopicQueue &topicQueue, int owPin)
      :Module(name, logging, topicQueue),
       owPin(owPin), oneWire(owPin), DS18B20(&oneWire)
       {}

//-------------------------------------------------------------------------------
//  1 Wire public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void OW::start() {
  Module::start();

  logging.info("starting 1 Wire");
  scanBus();
  if (count == 0)
    logging.error("no 1 Wire devices found!!");
  else
  logging.info("1 Wire scan done, found " + String(count) + " devices");
}

//...............................................................................
// handle
//...............................................................................
void OW::handle() {
  Module::handle();
  unsigned long now = millis();

//poll measurement values
  if (now - tPoll > OWPOLL){
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
}

//...............................................................................
//  read DS18B20
//...............................................................................
void OW::readDS18B20() {
  DynamicJsonBuffer root;
  JsonObject& sensoren = root.parseObject(sensorenJson);

  bool changed = false;
  scanBus();
  if (countOld != count) changed = true;

  String eventPrefix= "~/event/device/" + String(name) + "/";
  for (int i = 0; i < count; i++) {
    DS18B20.getAddress(DS18B20device, i);
    delay(100);

    String deviceValue = "";
    char str[5];
    for (int j = 0; j < 8; j++) {
      sprintf(str, "%02X", DS18B20device[j]);
      deviceValue += str;
      if (j < 7) deviceValue += "-";
    }

    if (!sensoren.containsKey(deviceValue)) {
      sensoren[deviceValue] = "";
      changed = true;
    }

    deviceValue += " ";
    deviceValue += String(DS18B20.getTempCByIndex(i));
    DS18B20.requestTemperatures();

    logging.debug(deviceValue);
    topicQueue.put(eventPrefix + "/" + deviceValue);

  }
  if (changed) {
    sensorenJson = "";
    sensoren.printTo(sensorenJson);
    //sensoren.prettyPrintTo(Serial);
    topicQueue.put("~/event/device/sensorsChanged " + sensorenJson);
  }

}
//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------
