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
// set callback
//...............................................................................
void OW::set_callbacks(String_CallbackFunction sensorChanged,
                       String_CallbackFunction sensorData) {

  on_SensorChanged = sensorChanged;
  on_SensorData = sensorData;
}

//...............................................................................
// start
//...............................................................................
void OW::start() {
  Module::start();
  scanBus();
}

//...............................................................................
// handle
//...............................................................................
void OW::handle() {
  Module::handle();
  unsigned long now = millis();

//poll measurement values
  //if (now - tPoll > owPoll){
    //tPoll = now;
    readDS18B20();
  //}
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
  JsonObject& sensors = root.createObject();
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_sensors = root.parseObject(ffs.deviceCFG.root);

  DynamicJsonBuffer data_root;
  JsonObject& data_sensors = data_root.createObject();

  scanBus();
  delay(200);

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

    //publish
    //assemble json
    sensors[strAddr] = "";        //add item
    //assemble data_json
    data_sensors[strAddr] = temp; //add item

  }
  DS18B20.requestTemperatures();



  //publish messured data
  sensorsJson = "";
  data_sensors.printTo(sensorsJson);
  //data_sensors.prettyPrintTo(Serial);
  topicQueue.put("~/event/device/sensorsData " + sensorsJson);
  if (on_SensorData != nullptr) on_SensorData(sensorsJson);  //callback event

  //publish sensors if items changed
  if (changed) {
    sensorsJson = "";
    sensors.printTo(sensorsJson);
    //sensors.prettyPrintTo(Serial);
    topicQueue.put("~/event/device/sensorsChanged " + sensorsJson);
    if (on_SensorChanged != nullptr) on_SensorChanged(sensorsJson);  //callback event

  }


}
