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
            mcpGPIO("MCP23017", logging, topicQueue, MCPIRQ, SDA, SCL)
            {}

//...............................................................................
// device start
//...............................................................................
void customDevice::start() {
  Device::start();
  logging.info("starting " + mcpGPIO.getVersion()); //only first time a class is started
  mcpGPIO.start();
  configMCP();

  logging.info("device running");

  logging.info("scanning I2C-Bus for devices");
  logging.info(Wire.i2c.scanBus());
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void customDevice::handle() {
  mcpGPIO.handle();
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
    mcpGPIO.mcp.digitalWrite(topic.getArgAsLong(0), topic.getArgAsLong(1));
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

  if (topic.modifyTopic(0) == "event/wifi/wl_connected"){
    //lcd.println(WiFi.localIP().toString(), 0);
    mcpGPIO.mcp.digitalWrite(8, true);
  }
}

//-------------------------------------------------------------------------------
//  Device private
//-------------------------------------------------------------------------------
//...............................................................................
// config MCP23017
//...............................................................................
void customDevice::configMCP() {
// IODIRx [RW] Datenrichtungsregister der GPIO-Ports:
  // 1 = INPUT; 0 = OUTPUT
  Wire.i2c.write8_8(MCP23017_IODIRA, B11111111);
  Wire.i2c.write8_8(MCP23017_IODIRB, B00000000);

// IPOLx [RW] Polarität
  // 1 = invertiert; 0 = nicht invertiert
  Wire.i2c.write8_8(MCP23017_IPOLA, B11111111);
  //Wire.i2c.write8_8(MCP23017_IPOLB, B11111111);

// GPINTENx [RW] Interrupt-On-Change-Funktion
  // 1 = IRQ enabled; 0 = IRQ disabled
  // Es müssen zusätzlich die DEFVAL- und INTCON-Register konfiguriert werden.
  Wire.i2c.write8_8(MCP23017_GPINTENA, B11111111);
  //Wire.i2c.write8_8(MCP23017_GPINTENB, B11111111);

// DEFVALx [RW] IRQ-Vergleichsregister
  // Vergleich GPIO mit DEFVALx bei Opposition und aktivem IRQ über GPINTEN und INTCON wird ein Interrupt ausgelöst.
  Wire.i2c.write8_8(MCP23017_DEFVALA, B00000000);
  //Wire.i2c.write8_8(MCP23017_DEFVALB, B00000000);

// INTCONx [RW] Interruptmode
  // 1 = vergleich mit DEFVALx; 0 = Interrupt-On-Change
  Wire.i2c.write8_8(MCP23017_INTCONA, B00000000);
  //Wire.i2c.write8_8(MCP23017_INTCONB, B00000000);

// GPPUx [RW] INPUT Pull-Up 100k
  // 1 = enabled, 0 = disabled
  Wire.i2c.write8_8(MCP23017_GPPUA, B11111111);
  //Wire.i2c.write8_8(MCP23017_GPPUB, B11111111);

// IOCON [RW] IO-Konfigurationsregister
  Wire.i2c.write8_8(MCP23017_IOCONA, B01000000);
  //                   BANK          ─┘│││││││
  //                   MIRROR        ──┘││││││
  //                   SEQOP         ───┘│││││
  //                   DISSLW        ────┘││││
  //                   HAEN          ─────┘│││
  //                   ODR           ──────┘││
  //                   INTPOL        ───────┘│
  //                   allways 0     ────────┘
}
