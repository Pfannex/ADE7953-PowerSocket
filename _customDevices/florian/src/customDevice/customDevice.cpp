#include "customDevice.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  constructor
//-------------------------------------------------------------------------------
customDevice::customDevice(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
    : Device(logging, topicQueue, ffs),
      mcpGPIO("MCP23017", logging, topicQueue, MCPIRQ, SDA, SCL),
      Drawer_01("ws2812", logging, topicQueue, PIN_WS2812, LEDSCOUNT)
 {

  type = String(DEVICETYPE);
  version = String(DEVICEVERSION);
}

//...............................................................................
// device start
//...............................................................................

void customDevice::start() {

  Device::start(); // mandatory

  Wire.i2c.scanBus();
  mcpGPIO.start();
  configMCP();
  Drawer_01.start();

  // ... your code here ...
  configItem = ffs.deviceCFG.readItem("configItem").toInt();
  logging.info("configItem is "+String(configItem));
  logging.info("device running");
}

//...............................................................................
// measure
//...............................................................................

float customDevice::measure() {
  return 0.815;
}

void customDevice::inform() {
  topicQueue.put("~/event/device/sensor1", measure());
  topicQueue.put("~/event/device/sensor2 foobar");
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................

void customDevice::handle() {
  mcpGPIO.handle();

  unsigned long now = millis();
  if (now - lastPoll >= 3000) {
    lastPoll = now;
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
    └─yourItem         (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/yourItem
    return TOPIC_NO;
  if (topic.itemIs(3, "yourItem")) {
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
    └─sensor1          (level 3)
  */

  logging.debug("device get topic " + topic.topic_asString());

  if (topic.getItemCount() != 4) // ~/get/device/sensor1
    return TOPIC_NO;
  if (topic.itemIs(3, "pressure")) {
    return pressure;
  } else {
    return TOPIC_NO;
  }
}

//...............................................................................
// event handler - called by the controller after receiving a topic (event)
//...............................................................................
void customDevice::on_events(Topic &topic) {

  // central business logic
  //Serial.println("customDevice::on_events | " + topic.modifyTopic(1));

  //5s Timer
  if (topic.modifyTopic(0) == "event/timer/shortUpdate") {
    //Serial.println("customDevice::on_events | ~/event/timer/longUpdate");
    readBMP180("Sensor_01");
    readSi7021("Sensor_02");
  }

  if (topic.modifyTopic(0) == "event/timer/1sUpdate"){
    //lcd.println(WiFi.localIP().toString(), 0);
    //logging.error("WiFi is connected");
    mcpGPIO.mcp.digitalWrite(8, true);
    delay(100);
    mcpGPIO.mcp.digitalWrite(8, false);

    //Drawer_01.WS2812_setColor(R,G,B);
    R += 10;
    G += 10;
    //B += 10;

    color = rand() % 0xFFFFFF + 1;

    //Drawer_01.ws2812.clear();
    for (uint16_t i = 0; i < LEDSCOUNT; i++) {

      //Drawer_01.ws2812.setPixelColor(i, R,G,B);
      //Drawer_01.ws2812.show();
      Drawer_01.ws2812.setPixelColor(i,color);
      Drawer_01.ws2812.show();
      //delay(10);
    }

    //Serial.println(mcpGPIO.mcp.readRegister(MCP23017_GPIOA), BIN);
    //Serial.println(mcpGPIO.mcp.readRegister(MCP23017_GPIOA));
    //delay(100);

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
   pressure = String(bmp.readPressure()/100);
   value = "pressure " + pressure;
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
