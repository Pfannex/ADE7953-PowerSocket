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

  DS18B20.begin();
  delay(100);
  DS18B20.setResolution(12);  //##
  delay(100);
  scanBus();
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
      deviceValue += " ";
      deviceValue += String(DS18B20.getTempCByIndex(i));
      DS18B20.requestTemperatures();

      logging.debug(deviceValue);
      topicQueue.put(eventPrefix + "/" + deviceValue);
    }
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
  logging.info("scanning 1Wire bus");

  count = DS18B20.getDeviceCount();
  delay(500);
  DS18B20.requestTemperatures();
  delay(500);

  if (count == 0)
    logging.error("no 1 Wire devices found!!");
  else
  logging.info("1 Wire scan done, found " + String(count) + " devices");
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------
