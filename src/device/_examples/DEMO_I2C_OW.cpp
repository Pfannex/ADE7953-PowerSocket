#include "DEMO_I2C_OW.h"
#include <Arduino.h>

//===============================================================================
//  Device
//===============================================================================

//-------------------------------------------------------------------------------
//  DeviceDEMO_I2C_OW
//-------------------------------------------------------------------------------
DEMO_I2C_OW::DEMO_I2C_OW(LOGGING &logging, TopicQueue &topicQueue, FFS &ffs)
           :Device(logging, topicQueue, ffs),
            i2c("i2c", logging, topicQueue, SDA, SCL),
            lcd("SSD1306", logging, topicQueue, SDA, SCL),
            ow("oneWire", logging, topicQueue, OWPIN),
            mcpGPIO("MCP23017", logging, topicQueue, MCPIRQ, SDA, SCL)
            {}

//...............................................................................
// device start
//...............................................................................
void DEMO_I2C_OW::start() {
  Device::start();
  logging.info("starting device " + String(DEVICETYPE) + " v" + String(DEVICEVERSION));

  logging.info("starting " + i2c.getVersion()); //only first time a class is started
  i2c.start();
  logging.info("starting " + lcd.getVersion()); //only first time a class is started
  lcd.start();
  logging.info("starting " + ow.getVersion()); //only first time a class is started
  ow.start();
  //logging.info("starting " + mcpGPIO.getVersion()); //only first time a class is started
  //mcpGPIO.start();
  //configMCP();

  logging.info("device running");
  logging.info(ffs.deviceCFG.readItem("NEW"));
}

//...............................................................................
// handle - periodically called by the controller
//...............................................................................
void DEMO_I2C_OW::handle() {
  i2c.handle();
  lcd.handle();
  ow.handle();
  //mcpGPIO.handle();
}

//...............................................................................
//  Device set
//...............................................................................

String DEMO_I2C_OW::set(Topic &topic) {
  /*
  ~/set
  └─device             (level 2)
    └─power            (level 3)
  */

  logging.debug("device set topic " + topic.topic_asString() + " to " +
                topic.arg_asString());

  if (topic.getItemCount() != 4) // ~/set/device/power
    return TOPIC_NO;
  if (topic.itemIs(3, "power")) {
    //setPowerMode(topic.getArgAsLong(0));
    return TOPIC_OK;
  } else {
    return TOPIC_NO;
  }
}
//...............................................................................
//  Device get
//...............................................................................

String DEMO_I2C_OW::get(Topic &topic) {
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
void DEMO_I2C_OW::on_events(Topic &topic) {
  // central business logic

  if (topic.modifyTopic(0) == "event/wifi/connected"){
    lcd.println(ffs.cfg.readItem("wifi_ip"), ArialMT_Plain_16, 0);
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
void DEMO_I2C_OW::configMCP() {
/*
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  delay(100);
  digitalWrite(15, HIGH);

  // IODIRx [RW] Datenrichtungsregister der GPIO-Ports:
  // 1 = INPUT; 0 = OUTPUT
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRA, B11111111);
  //Serial.println(mcpGPIO.mcp.readRegister(MCP23017_IODIRA), BIN);
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRB, B11111111);

  // IPOLx [RW] Polarität
  // 1 = invertiert; 0 = nicht invertiert
  mcpGPIO.mcp.writeRegister(MCP23017_IPOLA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_IPOLB, B11111111);

  // GPINTENx [RW] Interrupt-On-Change-Funktion
  // 1 = IRQ enabled; 0 = IRQ disabled
  // Es müssen zusätzlich die DEFVAL- und INTCON-Register konfiguriert werden.
  mcpGPIO.mcp.writeRegister(MCP23017_GPINTENA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_GPINTENB, B11111111);

  // DEFVALx [RW] IRQ-Vergleichsregister
  // Vergleich GPIO mit DEFVALx bei Opposition und aktivem IRQ über GPINTEN und INTCON wird ein Interrupt ausgelöst.
  mcpGPIO.mcp.writeRegister(MCP23017_DEFVALA, B00000000);
  mcpGPIO.mcp.writeRegister(MCP23017_DEFVALB, B00000000);

  // INTCONx [RW] Interruptmode
  // 1 = vergleich mit DEFVALx; 0 = Interrupt-On-Change
  mcpGPIO.mcp.writeRegister(MCP23017_INTCONA, B00000000);
  mcpGPIO.mcp.writeRegister(MCP23017_INTCONB, B00000000);

  // GPPUx [RW] INPUT Pull-Up 100k
  // 1 = enabled, 0 = disabled
  mcpGPIO.mcp.writeRegister(MCP23017_GPPUA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_GPPUB, B11111111);

  // INTFx [R] Interrupt detected
  // 1 = IRQ detected; 0 = no IRQ detected

  // INTCAPx [R] GPIO-Interrupt-State
  // Das Register kann nur gelesen werden und es wird nur beim Auftreten eines Interrupts aktualisiert.
  // Die Registerwerte bleiben unverändert, bis der Interrupt über das Lesen von INTCAP oder GPIO gelöscht.

  // GPIOx [RW] GPIO-Portregister
  // Ein Lesen der Register entspricht dem Lesen der Portleitungen.
  // Ein Schreiben in diese Register entspricht dem Schreiben in die Ausgangs-Latches (OLAT).

  // OLATx [RW] Outputregister
  // Ein Lesen von diesen Registern liefert den zuletzt hinein geschriebenen Wert, nicht die an den Pins aliegenden Daten.
  // Ein Schreiben auf diese Register ändert die Ausgangs-Latches, die ihrerseits diejenigen Pins beeinflussen,
  // die als Ausgänge konfiguriert sind.

  // IOCON [RW] IO-Konfigurationsregister
  mcpGPIO.mcp.writeRegister(MCP23017_IOCONA, B01000000);

  // IOCON.7 Bank-Konfiguration
  //   Ist BANK = 1, werden die Register nach Ports getrennt.
  //   Die Register von PORT A belegen die Adressen 0x00 bis 0x0A und die Register von PORT B die Adressen
  //   von 0x10 bis 0x1A (linke Spalte der Tabelle).
  //   Ist BANK = 0, werden die A- und B-Register gepaart.
  //   Zum Beispiel hat IODIRA die Adresse 0x00 und IODIRB die darauf folgende Adresse 0x01 (rechte Spalte der Tabelle).

  //   BANK = 0
  //   Schreiben von 0x80 in die Adresse 0x0A (ICON), um das BANK-Bit auf "1" zu setzen.
  //   Nach Abschluß des Schreibvorgangs zeigt der interne Adressepointer nun auf die folgende Adresse 0x0B, die ungültig ist.

  // IOCON.6 MIRROR-Bit
  //   1 = MIRROR INTA & INTB; 0 = INTA und INTB separat

  // IOCON.5 SEQOP AUTO increment Address-Pointer
  //   1 = disabled; 0 = enabled

  // IOCON.4 DISSLW-Bit (Slew-Rate) auf der SDA-Leitung
  //   1 = nicht beeinflusst; 0 = beeinflusst

  // IOCON.3 HAEN-Bit Hardware-Adressierung bei SPI (MCP23S17 only)

  // IOCON.2 ODR-Bit INT-Pin-Open-Drain-Konfiguration
  //   1 = enabled; 0 = disabled Polarität wird von INTPOL-Bit festgelegt

  // IOCON.1 INTPOL-Bit INT-Pin-Polarität (wenn ODR-Bit = 0)
  //   1 = high-active; 0 = low-active

  // IOCON.0 allways 0






  //mirror INTA/INTB, floating, flank
  //mcpGPIO.mcp.setupInterrupts(true,false,LOW);
  //mcpGPIO.mcp.pinMode(2, INPUT);
  //mcpGPIO.mcp.pullUp(2, HIGH);
  //mcpGPIO.mcp.setupInterruptPin(2,FALLING);*/
}
