#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include <Wire.h>
#include "modules/oneWire.h"
#include "Adafruit_BMP085.h"
#include "Adafruit_Si7021.h"

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "heatSensor"
#define DEVICEVERSION "V1.00"

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  String fillDashboard();
  void on_events(Topic &topic);
  void on_SensorChanged(String json);
  void on_SensorData(String json);

private:
  OW ow;
  int sensorPollTime;
  unsigned long lastPoll = 0;
  String owSensors = "";
  int index = 1;

  void pub_deviceCFGItem();                    //publish key:value to edit
  void set_deviceCFGItem(String value);        //store key:value to ffs
  void updateDashboard(String json);           //update measured data
  void modifySensorConfiguration(String json); //modify ffs
  void modifyDashboard();                      //modify dynamicDashboard
  bool containsKey(String key);                //search for key in dynamicDashboard
  // grid for sensors
  //WidgetGrid* grid;

  void readBMP180(String name);
  void readSi7021(String name);
};
