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
/*
  if (requestRunning){
    if (DS18B20.isConversionComplete()){
      readDS18B20();
      requestRunning = false;
      Serial.println("request complete");
    }
  }else{
    //poll measurement values
    unsigned long now = millis();
    if (now - tPoll > owPoll){
      tPoll = now;
      requestData();
    }
  }*/
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
  Serial.println("OW::scanBus()");
  DS18B20.begin();
  DS18B20.setResolution(12);  //##
  count = DS18B20.getDS18Count();
  logging.debug("found DS18-devices = " + String(count));
  DynamicJsonBuffer root;
  JsonObject& sensors = root.createObject();

  if (dsDevices != 0) {
    delete [] dsDevices;
  }
  dsDevices = new DSdevice [count];

  for (size_t i = 0; i < count; i++) {
    DS18B20.getAddress(dsDevices[i].addr, i);
    dsDevices[i].strAddr = "";
    char str[5];
    for (size_t j = 0; j < 8; j++) {
      sprintf(str, "%02X", dsDevices[i].addr[j]);
      dsDevices[i].strAddr += str;
      if (j < 7) dsDevices[i].strAddr += "-";
    }
    sensors[dsDevices[i].strAddr] = "";        //add item
  }

  String strSensors = "";
  sensors.printTo(strSensors);
  topicQueue.put("~/event/device/sensorsChanged " + strSensors);
  if (on_SensorChanged != nullptr) on_SensorChanged(strSensors);  //callback event

  //requestData();
}
/*
//...............................................................................
//  request Data from DS18
//...............................................................................
void OW::requestData() {
  requestRunning = true;
  unsigned long start = millis();
  DS18B20.setWaitForConversion(false);
  DS18B20.requestTemperatures();
  DS18B20.setWaitForConversion(true);
  logging.debug("request time = " + String(millis() - start) + "ms");
}

//...............................................................................
//  read DS18B20
//...............................................................................
void OW::readDS18B20() {
  DynamicJsonBuffer root;
  JsonObject& sensors = root.createObject();
  for (size_t i = 0; i < count; i++) {
    String temp = String(DS18B20.getTempC(dsDevice[i].addr));
    logging.debug(temp + "°C");
    sensors[dsDevice[i].strAddr] = temp;
  }
  String strSensors = "";
  sensors.printTo(strSensors);
  topicQueue.put("~/event/device/sensorsData " + strSensors);
  if (on_SensorData != nullptr) on_SensorData(strSensors);  //callback event
}
*/
