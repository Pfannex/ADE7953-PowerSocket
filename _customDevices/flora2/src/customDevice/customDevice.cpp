#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

const char *relays::names[] = {"pump",   "valve1", "valve2",
                               "valve3", "lamp",   "camera"};

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
  pressureSensorIsPresent = pressureSensor.begin(ADDRESS_PS);
  if (pressureSensorIsPresent) {
    count++;
    logging.info("BMP pressure sensor found at I2C address 0x" +
                 String(ADDRESS_PS, HEX));
  } else {
    logging.error("no BMP pressure sensor found at I2C address 0x" +
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
  uvSensorIsPresent = uvSensor.clearAck();
  if (uvSensorIsPresent) {
    count++;
    logging.info("VEML6070 uv sensor found");
  } else {
    logging.error("no VEML6070 uv sensor found");
  }

  // MCP23017
  mcp.begin(ADDRESS_MCP - 0x20);
  mcpIsPresent = true; // how to check?
  logging.info("MCP23017 found");
  for (int relay = 0; relay < RELAY_COUNT; relay++) {
    mcp.pinMode(relay, OUTPUT);
  }

  // ADS1115
  ads.begin();
  ads.setGain(GAIN_ONE); // 4.096 V
  adsIsPresent = true;   // how to check?
  adsGain_t gain = ads.getGain();
  logging.info("ADS1115 found");

  // poll if any sensors found
  if (count) {
    pollInterval = ffs.deviceCFG.readItem("pollInterval").toInt();
    if (pollInterval < 1000)
      pollInterval = POLL_IVL_DEF;
    logPollInterval();
  } else {
    logging.error("no sensors found, polling disabled");
  }

  // ready
  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

float customDevice::measureIlluminanceLux() {
  return lightSensor.readLightLevel();
}

float customDevice::measureTemperatureCelsius() {
  return 0.01 * round(100.0 * pressureSensor.readTemperature());
}

float customDevice::measurePressurehPa() {
  return 0.01 * round(pressureSensor.readPressure());
}
uint16_t customDevice::measureUVLevel() { return uvSensor.readUV(); }

float customDevice::measureUVmuWpercm2() { return measureUVLevel() * 5.0; }

float customDevice::measureVoltage(int channel) {
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
  //Di("UVreading", UVreading);
  for (i = 0; i < 4; i++) {
    //Di("risk_level_mapping_table[i]", risk_level_mapping_table[i]);
    if (UVreading <= risk_level_mapping_table[i]) {
      break;
    }
  }
  return i;
}

void customDevice::inform() {

  float temperature = measureTemperatureCelsius();
  float pressure = measurePressurehPa();
  float illuminance = measureIlluminanceLux();
  int uv = measureUVLevel();
  float uvIntensity = UVreadingToUVmuWpercm2(uv);
  uint16_t uvRiskLevel = UVreadingToUVRiskLevel(uv);
  float voltage[4];
  for (int channel = 0; channel < 4; channel++)
    voltage[channel] = measureVoltage(channel);

  topicQueue.put("~/event/device/temperature", temperature, "%.2f");
  topicQueue.put("~/event/device/pressure", pressure, "%.2f");
  topicQueue.put("~/event/device/illuminance", illuminance, "%.2f");
  topicQueue.put("~/event/device/uvIntensity", uvIntensity, "%g");
  topicQueue.put("~/event/device/uvRiskLevel", uvRiskLevel*1.0, "%g");
  topicQueue.put("~/event/device/voltage0", voltage[0], "%g");
  topicQueue.put("~/event/device/voltage1", voltage[1], "%g");
  topicQueue.put("~/event/device/voltage2", voltage[2], "%g");
  topicQueue.put("~/event/device/voltage3", voltage[3], "%g");
  logging.info("temperature  : " + String(temperature, 2) + " °C");
  logging.info("pressure     : " + String(pressure, 2) + " hPa");
  logging.info("illuminance  : " + String(illuminance, 2) + " lux");
  logging.info("uv intensity : " + String(uvIntensity, 2) + " µW/cm²");
  logging.info("uv risk level: " + String(uvRiskLevel));
  logging.info("voltage0     : " + String(voltage[0], 3) + " V");
  logging.info("voltage1     : " + String(voltage[1], 3) + " V");
  logging.info("voltage2     : " + String(voltage[2], 3) + " V");
  logging.info("voltage3     : " + String(voltage[3], 3) + " V");
}

//...............................................................................
// switch
//...............................................................................

void customDevice::switchRelay(int relay, int state) {
  Di("relay=", relay);
  Di("state=", state);
  // mcp.digitalWrite(relay, state);
  mcp.digitalWrite(0, state ? HIGH : LOW);
  topicQueue.put("~/event/device/" + String(relays::names[relay]) + " " +
                 String(state));
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
    // logging.debug("bing...");
    inform();
  }
}

//...............................................................................
//  Device set
//...............................................................................

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
