#include "customDevice.h"
#include "debug.h"
#include "framework/Utils/Debug.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

const char *relays::names[] = {"valve1", "valve2", "valve3",
                               "pump",   "lamp",   "camera"};

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs), pressureSensor(),
      lightSensor(ADDRESS_LS), uvSensor(), mcp(), ads(ADDRESS_ADS),
      button("button", logging, topicQueue, PIN_BUTTON),
      led("led", logging, topicQueue, PIN_LED, INVERSE) {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// helper
//...............................................................................

void customDevice::logPollInterval() {
  logging.info("polling sensor every " + String(pollInterval) + "ms");
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  // button
  logging.info("starting " +
               button.getVersion()); // only first time a class is started
  button.start();

  // LED
  led.start();
  setLedMode(0);

  // number of sensors
  int count = 0;

  // scanning I2C bus
  logging.info("scanning i2c bus, result:");
  logging.info(Wire.i2c.scanBus());

  // inherited start
  Device::start();
  logging.info("starting device " + String(DEVICETYPE));

  //
  // initialize sensors
  //

  // pressure sensor
  pressureSensorIsPresent = pressureSensor.begin(ADDRESS_PS, &Wire);
  if (pressureSensorIsPresent) {
    count++;
    logging.info("BME pressure sensor found at I2C address 0x" +
                 String(ADDRESS_PS, HEX));
    logging.info("BME Sensor ID is 0x" +
                 String(pressureSensor.sensorID(), HEX));
  } else {
    logging.error("no BME pressure sensor found at I2C address 0x" +
                  String(ADDRESS_PS, HEX));
  }

  // light sensor
  lightSensor.begin(RESOLUTION);
  lightSensorIsPresent = lightSensor.isPresent();
  if (lightSensorIsPresent) {
    count++;
    logging.info("BH1750 light sensor found at I2C address 0x" +
                 String(ADDRESS_LS, HEX));
  } else {
    logging.error("no BH1750 light sensor found at I2C address 0x" +
                  String(ADDRESS_LS, HEX));
  }

  // uv sensor
  uvSensor.begin(VEML6070_4_T);
  uvSensorIsPresent = Wire.i2c.isPresent(VEML6070_ADDR_L);
  uvSensor.clearAck();
  if (uvSensorIsPresent) {
    count++;
    logging.info("VEML6070 uv sensor found");
  } else {
    logging.error("no VEML6070 uv sensor found");
  }

  // MCP23017
  mcp.begin(ADDRESS_MCP - 0x20);
  mcpIsPresent = Wire.i2c.isPresent(ADDRESS_MCP);
  if (mcpIsPresent) {
    logging.info("MCP23017 device found at I2C address 0x" +
                 String(ADDRESS_MCP, HEX));
    for (int relay = 0; relay < RELAY_COUNT; relay++) {
      mcp.pinMode(relay, OUTPUT);
      mcp.digitalWrite(relay, HIGH); // HIGH = off
      relayState[relay] = 0;
    }
  } else {
    logging.error("no MCP23017 device found at I2C address 0x" +
                 String(ADDRESS_MCP, HEX));
  }

  // ADS1115
  adsIsPresent = Wire.i2c.isPresent(ADDRESS_ADS);
  if (adsIsPresent) {
    ads.begin();
    count++;
    logging.info("ADS1115 sensor found at I2C address 0x" +
                 String(ADDRESS_ADS, HEX));
    ads.setGain(GAIN_ONE); // 4.096 V
    adsIsPresent = Wire.i2c.isPresent(ADDRESS_ADS);
    adsGain_t gain = ads.getGain();
  } else {
    logging.error("no ADS1115 sensor found at I2C address 0x" +
                 String(ADDRESS_ADS, HEX));
  }

  // poll if any sensors found
  if (count) {
    pollInterval = ffs.deviceCFG.readItemLong("pollInterval");
    if (pollInterval < 200)
      pollInterval = POLL_IVL_DEF;
    logPollInterval();
  } else {
    logging.error("no sensors found, polling disabled");
  }

  // volume and moisture config
  getConfig();

  // ready
  logging.info("device running");
}

String customDevice::getDashboard() { return dashboard.asJsonDocument(); }

String customDevice::fillDashboard() {
  inform();
  informConfig();
  return TOPIC_OK;
}

//...............................................................................
// measure
//...............................................................................

float customDevice::measureIlluminanceLux() {
  return lightSensor.readLightLevel();
}

float customDevice::measureTemperatureCelsius() {
  // D(String(pressureSensor.readTemperature()).c_str());
  return 0.01 * round(100.0 * pressureSensor.readTemperature());
}

float customDevice::measurePressurehPa() {
  return 0.01 * round(pressureSensor.readPressure());
}

float customDevice::measureHumidityPercent() {
  return round(pressureSensor.readHumidity());
}

uint16_t customDevice::measureUVLevel() { return uvSensor.readUV(); }

float customDevice::measureUVmuWpercm2() { return measureUVLevel() * 5.0; }

float customDevice::measureVoltage(int channel) {
  // Di("measure voltage", channel);
  return ads.readADC_SingleEnded(channel) * 0.000125;
}

// https://www.vishay.com/docs/84277/veml6070.pdf, BASIC CHARACTERISTICS
float customDevice::UVreadingToUVmuWpercm2(uint16_t UVreading) {
  return UVreading * 5.0;
}

// https://www.vishay.com/docs/84310/designingveml6070.pdf
// 0= LOW, 1= MODERATE, 2= HIGH, 3= VERY_HIGH, 4= EXTREME
// Rset = 270 kOhm (274)
uint16_t customDevice::UVreadingToUVRiskLevel(uint16_t UVreading) {
  // 4T
  uint16_t risk_level_mapping_table[4] = {2240, 4482, 5976, 8216};
  uint16_t i;
  // Di("UVreading", UVreading);
  for (i = 0; i < 4; i++) {
    // Di("risk_level_mapping_table[i]", risk_level_mapping_table[i]);
    if (UVreading <= risk_level_mapping_table[i]) {
      break;
    }
  }
  return i;
}

float customDevice::voltageToVolumeLiter(float voltage) {
  return tVolume1 + tSlope * (voltage - tVoltage1);
}

float customDevice::voltageToMoisturePercent(float voltage) {
  return mMoisture1 + mSlope * (voltage - mVoltage1);
}

void customDevice::measure() {

  if (pressureSensorIsPresent) {
    temperature = measureTemperatureCelsius();
    pressure = measurePressurehPa();
    humidity = measureHumidityPercent();
  }
  if (lightSensorIsPresent) {
    illuminance = measureIlluminanceLux();
  }
  if (uvSensorIsPresent) {
    uv = measureUVLevel();
    uvIntensity = UVreadingToUVmuWpercm2(uv);
    uint16_t uvRiskLevel = UVreadingToUVRiskLevel(uv);
  }
  if (adsIsPresent) {
    for (int channel = 0; channel < 4; channel++)
      voltage[channel] = measureVoltage(channel);
    moisture = voltageToMoisturePercent(voltage[MOISTURE_VOLTAGE]);
    volume = voltageToVolumeLiter(voltage[TANK_VOLTAGE]);
  }
}

void customDevice::inform() {

  if (pressureSensorIsPresent) {
    topicQueue.put("~/event/device/temperature", temperature, "%.2f");
    topicQueue.put("~/event/device/pressure", pressure, "%.2f");
    topicQueue.put("~/event/device/humidity", humidity, "%.2f");
  }
  if (lightSensorIsPresent) {
    topicQueue.put("~/event/device/illuminance", illuminance, "%.2f");
  }
  if (uvSensorIsPresent) {
    topicQueue.put("~/event/device/uvIntensity", uvIntensity, "%g");
    topicQueue.put("~/event/device/uvRiskLevel", uvRiskLevel * 1.0, "%g");
  }
  if (adsIsPresent) {
    topicQueue.put("~/event/device/voltage0", voltage[0], "%g");
    topicQueue.put("~/event/device/voltage1", voltage[1], "%g");
    topicQueue.put("~/event/device/voltage2", voltage[2], "%g");
    topicQueue.put("~/event/device/voltage3", voltage[3], "%g");
    topicQueue.put("~/event/device/moisture", moisture, "%g");
    topicQueue.put("~/event/device/volume", volume, "%g");
  }
  if (mcpIsPresent) {
    for (int relay = 0; relay < RELAY_COUNT; relay++) {
      topicQueue.put("~/event/device/" + String(relays::names[relay]) + " " +
                     String(relayState[relay]));
    }
  }
  topicQueue.put("~/event/device/config_voltage", getConfigVoltage(configPoint),
                 "%g");

  /* too much for queue
     logging.info("temperature  : " + String(temperature, 2) + " °C");
     logging.info("pressure     : " + String(pressure, 2) + " hPa");
     logging.info("illuminance  : " + String(illuminance, 2) + " lux");
     logging.info("uv intensity : " + String(uvIntensity, 2) + " µW/cm²");
     logging.info("uv risk level: " + String(uvRiskLevel));
     logging.info("voltage0     : " + String(voltage[0], 3) + " V");
     logging.info("voltage1     : " + String(voltage[1], 3) + " V");
     logging.info("voltage2     : " + String(voltage[2], 3) + " V");
     logging.info("voltage3     : " + String(voltage[3], 3) + " V");
     logging.info("moisture     : " + String(moisture, 0) + " %");
     logging.info("volume       : " + String(volume, 0) + " l");
   */
}

//...............................................................................
// switch
//...............................................................................

void customDevice::switchRelay(int relay, int state) {
  if (mcpIsPresent) {
    mcp.digitalWrite(relay, state ? LOW : HIGH);
    relayState[relay] = state;
    topicQueue.put("~/event/device/" + String(relays::names[relay]) + " " +
                   String(state));
  }
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {

  button.handle();
  led.handle();

  if (!pollInterval)
    return;
  unsigned long now = millis();
  if (now - lastPoll >= pollInterval) {
    lastPoll = now;
    measure();
    inform();
  }
}

//...............................................................................
//  Device set
//...............................................................................

void customDevice::setConfig() {
  ffs.deviceCFG.writeItemFloat("tVoltage1", tVoltage1);
  ffs.deviceCFG.writeItemFloat("tVolume1", tVolume1);
  ffs.deviceCFG.writeItemFloat("tVoltage2", tVoltage2);
  ffs.deviceCFG.writeItemFloat("tVolume2", tVolume2);
  ffs.deviceCFG.writeItemFloat("mVoltage1", mVoltage1);
  ffs.deviceCFG.writeItemFloat("mMoisture1", mMoisture1);
  ffs.deviceCFG.writeItemFloat("mVoltage2", mVoltage2);
  ffs.deviceCFG.writeItemFloat("mMoisture2", mMoisture2);
  ffs.deviceCFG.saveFile();
  calcConfig();
}

String customDevice::set(Topic &topic) {
  /*
     ~/set
     └─device             (level 2)
     └─gpio             (level 3)
     └─pollInterval     (level 3)
   */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/pollInterval
    return TOPIC_NO;
  if (topic.itemIs(3, "pollInterval")) {
    pollInterval = topic.getArgAsLong(0);
    logging.info("polling sensor every " + String(pollInterval) + "ms");
    ffs.deviceCFG.writeItemLong("pollInterval", pollInterval);
    ffs.deviceCFG.saveFile();
    return TOPIC_OK;
  } else if (topic.itemIs(3, "config_back")) {
    changeConfigPoint(configPoint - 1);
    topicQueue.put("~/event/device/dashboardChanged");
    return TOPIC_OK;
  } else if (topic.itemIs(3, "config_forward")) {
    changeConfigPoint(configPoint + 1);
    topicQueue.put("~/event/device/dashboardChanged");
    return TOPIC_OK;
  } else if (topic.itemIs(3, "config_value")) {
    setConfigValue(configPoint, topic.getArgAsFloat(0));
    return TOPIC_OK;
  } else if (topic.itemIs(3, "config_apply")) {
    configApply();
    return TOPIC_OK;
  } else {
    for (int relay = 0; relay < RELAY_COUNT; relay++) {
      if (topic.itemIs(3, relays::names[relay])) {
        int state = topic.getArgAsLong(0);
        if (state < 0 || state > 1)
          return TOPIC_NO;
        else {
          switchRelay(relay, state);
          return TOPIC_OK;
        }
      }
    }
  }
  return TOPIC_NO;
}

//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
     ~/get
     └─device             (level 2)
     illuminance            (level 3)
   */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/illuminance
    return TOPIC_NO;
  if (topic.itemIs(3, "pressure")) {
    return String(measurePressurehPa()); // result in hPa
  } else if (topic.itemIs(3, "temperature")) {
    return String(measureTemperatureCelsius()); // result in °C
  } else if (topic.itemIs(3, "i2cbus")) {
    return String(Wire.i2c.scanBus());
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

  // listen to ~/device/led/setmode
  if (led.isForModule(topic)) {
    if (led.isItem(topic, "setmode"))
      setLedMode(topic.getArgAsLong(0));
  }

  // central business logic
  if (button.isForModule(topic)) {
    // events from button
    //
    // - click
    if (button.isItem(topic, "click")) {
      // -- short
      if (topic.argIs(0, "short")) {
        if (configMode)
          setConfigMode(0);
        else
          inform();
      }
      // -- long
      if (topic.argIs(0, "long"))
        setConfigMode(!configMode);
    }
    // - idle
    if (button.isItem(topic, "idle"))
      setConfigMode(0);
  }
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------

//...............................................................................
//  configuration
//...............................................................................

void customDevice::calcConfig() {

  tSlope = tVolume2 - tVolume1;
  if (tVoltage2 != tVoltage1)
    tSlope /= (tVoltage2 - tVoltage1);
  logging.info("volume config: (" + String(tVoltage1, 3) + "," +
               String(tVolume1, 0) + "), (" + String(tVoltage2, 3) + "," +
               String(tVolume2, 0) + "), " + String(tSlope, 1));

  mSlope = mMoisture2 - mMoisture1;
  if (mVoltage2 != mVoltage1)
    mSlope /= (mVoltage2 - mVoltage1);
  logging.info("moisture config: (" + String(mVoltage1, 3) + "," +
               String(mMoisture1, 0) + "), (" + String(mVoltage2, 3) + "," +
               String(mMoisture2, 0) + "), " + String(mSlope, 1));
}

void customDevice::getConfig() {
  tVoltage1 = ffs.deviceCFG.readItemFloat("tVoltage1");
  tVoltage2 = ffs.deviceCFG.readItemFloat("tVoltage2");
  tVolume1 = ffs.deviceCFG.readItemFloat("tVolume1");
  tVolume2 = ffs.deviceCFG.readItemFloat("tVolume2");

  mVoltage1 = ffs.deviceCFG.readItemFloat("mVoltage1");
  mVoltage2 = ffs.deviceCFG.readItemFloat("mVoltage2");
  mMoisture1 = ffs.deviceCFG.readItemFloat("mMoisture1");
  mMoisture2 = ffs.deviceCFG.readItemFloat("mMoisture2");

  calcConfig();
}

void customDevice::informConfig() {
  topicQueue.put("~/event/device/config_value", configValue, "%g");
}

void customDevice::changeConfigPoint(int newConfigPoint) {

  const char *points[] = {"Moisture Point 1", "Moisture Point 2",
                          "Volume Point 1", "Volume Point 2"};
  const char *topics[] = {"moisture1", "moisture2", "volume1", "volume2"};

  newConfigPoint += 4; // avoid negative remainders
  newConfigPoint %= 4; // remainder, not modulo
  if (configPoint != newConfigPoint) {
    configPoint = newConfigPoint;
    logging.debug("change to config point " + String(configPoint));
    Widget *w;
    WidgetGroup *g;
    // title of group
    w = dashboard.findWidget("config");
    if (w) {
      g = (WidgetGroup *)w;
      g->collapsed = "false";
      w = g->findWidget("config_group");
      if (w) {
        g = (WidgetGroup *)w;
        g->caption = points[configPoint];
        setConfigValue(configPoint, getConfigValue(configPoint));
      } else {
        SOFTWAREERROR;
      }
    } else {
      SOFTWAREERROR;
    }
  }
}

void customDevice::configApply() {
  switch (configPoint) {
  case 0:
    mVoltage1 = getConfigVoltage(0);
    mMoisture1 = configValue;
    break;
  case 1:
    mVoltage2 = getConfigVoltage(1);
    mMoisture2 = configValue;
    break;
  case 2:
    tVoltage1 = getConfigVoltage(2);
    tVolume1 = configValue;
    break;
  case 3:
    tVoltage2 = getConfigVoltage(3);
    tVolume2 = configValue;
    break;
  }
  setConfig();
}

float customDevice::getConfigVoltage(int configPoint) {
  switch (configPoint) {
  case 0:
  case 1:
    return voltage[MOISTURE_VOLTAGE];
    break;
  case 2:
  case 3:
    return voltage[TANK_VOLTAGE];
    break;
  }
}

float customDevice::getConfigValue(int configPoint) {
  switch (configPoint) {
  case 0:
    return mMoisture1;
    break;
  case 1:
    return mMoisture2;
    break;
  case 2:
    return tVolume1;
    break;
  case 3:
    return tVolume2;
    break;
  }
}

void customDevice::setConfigValue(int configPoint, float value) {
  configValue = value;
  logging.debug("config value for config point " + String(configPoint) +
                " set to " + String(configValue));
}

//...............................................................................
//  mode setter
//...............................................................................

void customDevice::setConfigMode(int value) {
  if (configMode == value)
    return;
  configMode = value;
  topicQueue.put("~/event/device/configMode", configMode);

  if (configMode == 1) {
    setLedMode(2);
  } else {
    setLedMode(0);
  }
}

void customDevice::setLedMode(int value) {
  switch (value) {
  case 0:
    led.setOutputMode(OFF);
    break;
  case 1:
    led.setOutputMode(ON);
    break;
  case 2:
    led.setOutputMode(BLINK, 100);
    break;
  case 3:
    led.setOutputMode(BLINK, 1000);
    break;
  }
}
