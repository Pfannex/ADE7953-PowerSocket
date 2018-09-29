#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  DevicecustomDevice
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :Device(logging, topicQueue, ffs),

            lcd("SSD1306", logging, topicQueue, SDA, SCL)
            //ow("oneWire", logging, topicQueue, OWPIN),
            //mcpGPIO("MCP23017", logging, topicQueue, MCPIRQ, SDA, SCL)
            {}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {
  Device::start();
  sensorPollTime = ffs.deviceCFG.readItem("I2CPOLL").toInt();
  logging.info("I2C sensors polling time: " + String(sensorPollTime));

  logging.info("starting device " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  logging.info("starting " + lcd.getVersion()); //only first time a class is started
  lcd.start();

  //logging.info("starting " + ow.getVersion()); //only first time a class is started
  //ow.start();
  logging.info("starting " + mcpGPIO.getVersion()); //only first time a class is started
  //mcpGPIO.start();
  //configMCP();

  logging.info("device running");
  logging.info(ffs.deviceCFG.readItem("NEW"));

  logging.info("scanning I2C-Bus for devices");
  logging.info(Wire.i2c.scanBus());

  //Wire.i2c.testI2C();
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  unsigned long now = millis();

  lcd.handle();
  //ow.handle();
  //mcpGPIO.handle();

  if (now - lastPoll > sensorPollTime){
    lastPoll = now;
    readBMP180("sensor1");
    readSi7021("sensor2");
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
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/gpio
    return TOPIC_NO;
  if (topic.itemIs(3, "gpio")) {
    //mcpGPIO.mcp.digitalWrite(topic.getArgAsLong(0), topic.getArgAsLong(1));
    //setPowerMode(topic.getArgAsLong(0));
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  Device get
//...............................................................................

String customDevice::get(Topic &topic) {
  /*
  ~/get
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //topicQueue.put("~/event/device/power", power);
    //return String(power);
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// Eventhandler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {
  // central business logic

  if (topic.modifyTopic(0) == "event/wifi/connected"){
    //lcd.println(WiFi.localIP().toString(), 0);
    //mcpGPIO.mcp.digitalWrite(8, true);
  }

/*
//button
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
          setPowerMode(!power);
      }
      // -- long
      if (topic.argIs(0, "long"))
        setConfigMode(!configMode);
    }
    // - idle
    if (button.isItem(topic, "idle"))
      setConfigMode(0);
  }
*/

}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
// config MCP23017
//...............................................................................
/*
void customDevice::configMCP() {
  // IODIRx [RW] Datenrichtungsregister der GPIO-Ports:
  // 1 = INPUT; 0 = OUTPUT
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRB, B00000000);
  // IPOLx [RW] Polarität
  // 1 = invertiert; 0 = nicht invertiert
  mcpGPIO.mcp.writeRegister(MCP23017_IPOLA, B11111111);
  //mcpGPIO.mcp.writeRegister(MCP23017_IPOLB, B11111111);
  // GPINTENx [RW] Interrupt-On-Change-Funktion
  // 1 = IRQ enabled; 0 = IRQ disabled
  // Es müssen zusätzlich die DEFVAL- und INTCON-Register konfiguriert werden.
  mcpGPIO.mcp.writeRegister(MCP23017_GPINTENA, B11111111);
  //mcpGPIO.mcp.writeRegister(MCP23017_GPINTENB, B11111111);
  // DEFVALx [RW] IRQ-Vergleichsregister
  // Vergleich GPIO mit DEFVALx bei Opposition und aktivem IRQ über GPINTEN und INTCON wird ein Interrupt ausgelöst.
  mcpGPIO.mcp.writeRegister(MCP23017_DEFVALA, B00000000);
  //mcpGPIO.mcp.writeRegister(MCP23017_DEFVALB, B00000000);
  // INTCONx [RW] Interruptmode
  // 1 = vergleich mit DEFVALx; 0 = Interrupt-On-Change
  mcpGPIO.mcp.writeRegister(MCP23017_INTCONA, B00000000);
  //mcpGPIO.mcp.writeRegister(MCP23017_INTCONB, B00000000);
  // GPPUx [RW] INPUT Pull-Up 100k
  // 1 = enabled, 0 = disabled
  mcpGPIO.mcp.writeRegister(MCP23017_GPPUA, B11111111);
  //mcpGPIO.mcp.writeRegister(MCP23017_GPPUB, B11111111);
  // IOCON [RW] IO-Konfigurationsregister
  mcpGPIO.mcp.writeRegister(MCP23017_IOCONA, B01000000);
  //                           BANK          ─┘│││││││
  //                           MIRROR        ──┘││││││
  //                           SEQOP         ───┘│││││
  //                           DISSLW        ────┘││││
  //                           HAEN          ─────┘│││
  //                           ODR           ──────┘││
  //                           INTPOL        ───────┘│
  //                           allways 0     ────────┘
}
*/

//...............................................................................
// read BMP180
//...............................................................................
void customDevice::readBMP180(String name) {
   Adafruit_BMP085 bmp;
   String eventPrefix= "~/event/device/" + name + "/";

   bmp.begin();
   String value = "temperature " + String(bmp.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "pressure " + String(bmp.readPressure()/100);
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}

//...............................................................................
// read SI7021
//...............................................................................
void customDevice::readSi7021(String name) {
   Adafruit_Si7021 si;
   String eventPrefix= "~/event/device/" + name + "/";

   si.begin();
   String value = "temperature " + String(si.readTemperature());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "humidity " + String(si.readHumidity());
   //logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}
