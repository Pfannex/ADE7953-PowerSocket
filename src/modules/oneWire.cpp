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

  if (requestRunning){
    //runs++;
    if (DS18B20.isConversionComplete()){
      //readData
      //Serial.println("runs before = " + String(runs));
      //runs = 0;
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
  Serial.println("OW::scanBus()");
  unsigned long start = millis();
  DS18B20.begin();
  DS18B20.setResolution(12);  //##
  countOld = count;
  count = DS18B20.getDS18Count();
  logging.debug("scanBus time = " + String(millis() - start) + "ms");
  logging.debug("found DS18-devices = " + String(count));

  Serial.println("sizeOf dsDevices = " + String(sizeof(dsDevices)));
  if (dsDevices != 0) {
    Serial.println("delete [] dsDevices");
    delete [] dsDevices;
  }
  Serial.println("sizeOf dsDevices = " + String(sizeof(dsDevices)));
  dsDevices = new DSdevice [count];
  Serial.println("sizeOf dsDevices = " + String(sizeof(dsDevices)));

  //dsDevices[0].strAddr = "String1";
  //dsDevices[0].addr[0] = 55;
  //dsDevices[0].value = 33;
  //Serial.println(dsDevices[0].strAddr + "-" + String(dsDevices[0].addr[0]) + "-" + String(dsDevices[0].value));

  start = millis();
  //store device adresses
  DynamicJsonBuffer root;
  JsonObject& sensors = root.createObject();

  for (size_t i = 0; i < count; i++) {
    //DeviceAddress addr;
    DS18B20.getAddress(dsDevices[i].addr, i);
    dsDevices[i].strAddr = "";
    char str[5];
    for (size_t j = 0; j < 8; j++) {
      sprintf(str, "%02X", dsDevices[i].addr[j]);
      dsDevices[i].strAddr += str;
      if (j < 7) dsDevices[i].strAddr += "-";
    }
    logging.debug(dsDevices[i].strAddr);
    sensors[dsDevices[i].strAddr] = "";        //add item
  }
  logging.debug("getAddr time = " + String(millis() - start) + "ms");

  String strSensors = "";
  sensors.printTo(strSensors);
  //Serial.println("sensorsChanged");
  //sensors.prettyPrintTo(Serial);
  //Serial.println(strSensors);
  topicQueue.put("~/event/device/sensorsChanged " + strSensors);
  if (on_SensorChanged != nullptr) on_SensorChanged(strSensors);  //callback event

  //controll async in module handle!!!
  requestData();



}

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
  unsigned long start = millis();
  DynamicJsonBuffer root;
  JsonObject& sensors = root.createObject();
  for (size_t i = 0; i < count; i++) {
    String temp = String(DS18B20.getTempC(dsDevices[i].addr));
    logging.debug(temp + "°C");
    sensors[dsDevices[i].strAddr] = temp;
  }
  logging.debug("getTemp time = " + String(millis() - start) + "ms");
  String strSensors = "";
  sensors.printTo(strSensors);
  //Serial.println("sensorsData");
  //sensors.prettyPrintTo(Serial);
  //Serial.println("");
  topicQueue.put("~/event/device/sensorsData " + strSensors);
  if (on_SensorData != nullptr) on_SensorData(strSensors);  //callback event


/*
  DynamicJsonBuffer root;
  JsonObject& sensors = root.createObject();
  DynamicJsonBuffer ffs_root;
  JsonObject& ffs_sensors = root.parseObject(ffs.deviceCFG.root);

  DynamicJsonBuffer data_root;
  JsonObject& data_sensors = data_root.createObject();

  //scanBus();
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
*/

}
