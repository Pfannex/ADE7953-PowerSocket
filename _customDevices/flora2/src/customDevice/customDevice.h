#include "framework/OmniESP/Device.h"

#include "framework/Core/FFS.h"
#include "framework/OmniESP/Topic.h"
#include "framework/Utils/Logger.h"

// device-specific setup
#include "customDeviceSetup.h"

// modules required by device
#include <Wire.h>
#include "AS_BH1750.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_ADS1015.h"
#include "Adafruit_BME280.h"
#include "Adafruit_MCP23017.h"
#include "Adafruit_VEML6070.h"
#include "modules/GPIO.h"

//###############################################################################
//  Device
//###############################################################################

#define DEVICETYPE "flora"
#define DEVICEVERSION "v2"

// maintainer: Boris Neubert

class customDevice : public Device {

public:
  customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs);
  void start();
  void handle();
  String set(Topic &topic);
  String get(Topic &topic);
  void on_events(Topic &topic);
  String getDashboard();
  String fillDashboard();

private:

  void inform();
  void logPollInterval();

  // sensors
  Adafruit_BME280 pressureSensor;
  AS_BH1750 lightSensor;
  Adafruit_VEML6070 uvSensor;
  Adafruit_MCP23017 mcp;
  Adafruit_ADS1115 ads;

  bool pressureSensorIsPresent = false;
  bool lightSensorIsPresent = false;
  bool uvSensorIsPresent = false;
  bool mcpIsPresent = false;
  bool adsIsPresent = false;

  // polling
  unsigned long pollInterval = 0;
  unsigned long lastPoll = 0;

  // measurement
  float measureIlluminanceLux();
  float measureTemperatureCelsius();
  float measurePressurehPa();
  float measureHumidityPercent();
  uint16_t measureUVLevel();
  float measureUVmuWpercm2();
  uint16_t UVreadingToUVRiskLevel(uint16_t);
  float UVreadingToUVmuWpercm2(uint16_t);
  float voltageToVolumeLiter(float);
  float voltageToMoisturePercent(float);
  float measureVoltage(int channel);
  void measure();

  float temperature;
  float pressure;
  float humidity;
  float illuminance;
  int uv;
  float uvIntensity;
  uint16_t uvRiskLevel;
  float voltage[4];
  int relayState[RELAY_COUNT];
  float moisture;
  float volume;

  // relays
  void switchRelay(int relay, int state);

  GPIOinput button;
  int configMode = 0;

  GPIOoutput led;
  void setLedMode(int value);

  // config
  void setConfigMode(int value);

  // calibration
  int configPoint = 0;
  float configValue= 0.0;
  void changeConfigPoint(int newConfigPoint);
  void configApply();
  float getConfigVoltage(int configPoint);
  float getConfigValue(int configPoint);
  void setConfigValue(int configPoint, float value);
  void informConfig();
  void getConfig();
  void setConfig();
  void calcConfig();
  // config tank volume
  float tVoltage1, tVoltage2, tVolume1, tVolume2, tSlope;
  // config moisture sensor
  float mVoltage1, mVoltage2, mMoisture1, mMoisture2, mSlope;

};
