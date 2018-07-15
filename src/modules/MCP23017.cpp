#include "MCP23017.h"
#include <Arduino.h>

//===============================================================================
//  MCP23017
//===============================================================================
MCP23017::MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue,
                   int GPIOIrqPin, int sda, int scl)
         :Module(name, logging, topicQueue),
          irqPin(GPIOIrqPin), sda(sda), scl(scl)
         {}

//-------------------------------------------------------------------------------
//  MCP23017 public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void MCP23017::start() {
  Module::start();
  logging.info("setting GPIO pin " + String(irqPin) + " to IRQ-Pin");
  pinMode(irqPin, INPUT_PULLUP);
  irqSetMode(FALLING);
}

//...............................................................................
// handle
//...............................................................................
void MCP23017::handle() {
  Module::handle();
  irqHandle();
}

//...............................................................................
// getVersion
//...............................................................................
String MCP23017::getVersion() {
  return  String(MCP23017_Name) + " v" + String(MCP23017_Version);
}

//...............................................................................
// IRQ
//...............................................................................
void MCP23017::irq() {
  irqDetected = true;
}

void MCP23017::irqSetMode(int mode){
  if (mode == 4){
    detachInterrupt(irqPin);
  }else{
    //LOW     = 0, HIGH    = 1, RISING  = 1, FALLING = 2
    //CHANGE  = 3, OFF     = 4
    attachInterrupt(digitalPinToInterrupt(irqPin), std::bind(&MCP23017::irq, this), mode);
  }
}

void MCP23017::irqHandle() {
  unsigned long now = millis();
  String eventPrefix= "~/event/device/" + String(name) + "/";

// main handling
  if (irqDetected){
    irqSetMode(irqOFF);
    irqDetected = false;
    lastIrqTime = now;
    //Serial.print("  INTFA -   ");Serial.println(mcp.readRegister(MCP23017_INTFA), BIN);
    //Serial.print("  INTCAPA - ");Serial.println(mcp.readRegister(MCP23017_INTCAPA), BIN);
    //Serial.print("  GPIOA -   ");Serial.println(mcp.readRegister(MCP23017_GPIOA), BIN);
    //Serial.print("  INTFA -   ");Serial.println(mcp.readRegister(MCP23017_INTFA), BIN);
    //Serial.print("  INTCAPA - ");Serial.println(mcp.readRegister(MCP23017_INTCAPA), BIN);
    //logging.debug("MCP23017 IRQ");

    uint8_t pin = mcp.getLastInterruptPin();
    uint8_t val = mcp.getLastInterruptPinValue();
    uint8_t state = mcp.digitalRead(pin);

    logging.debug("mcpGPIO " + String(pin) + " | " + String(val));
    topicQueue.put(eventPrefix + "/" + String(pin) + " " + String(val));

    clearIRQ();

/*
    while (mcp.readRegister(MCP23017_INTFA) > 0 || mcp.readRegister(MCP23017_INTFB) > 0){
      Serial.println("  hanging.....");
      //Serial.print("INTFA - ");Serial.println(MCP23017.readRegister(MCP23017_INTFA), BIN);
      //Serial.print("INTFB - ");Serial.println(MCP23017.readRegister(MCP23017_INTFB), BIN);
      Serial.print("  GPIOA - ");Serial.println(mcp.readRegister(MCP23017_GPIOA), BIN);
      Serial.print("  GPIOB - ");Serial.println(mcp.readRegister(MCP23017_GPIOB), BIN);
    }
*/
    irqSetMode(FALLING);
  }

//debouncing
  if (now - lastIrqTime > MCPDEBOUNCETIME && lastIrqTime > 0){
    //logging.debug("debounce enable IRQ");
    irqDetected = false;  //clear accrued IRQs during debouncing
    lastIrqTime = 0;
    irqSetMode(FALLING);
  }

}
//-------------------------------------------------------------------------------
//  MCP23017 private
//-------------------------------------------------------------------------------

//...............................................................................
// clear MCP23017 IRQ
//...............................................................................
void MCP23017::clearIRQ() {
/*
  while (mcp.readRegister(MCP23017_INTFA) > 0 || mcp.readRegister(MCP23017_INTFB) > 0){
    mcp.readRegister(MCP23017_GPIOA);
    mcp.readRegister(MCP23017_GPIOB);
    //Serial.println("---------------------------------------------");
    //Serial.println("  hanging.....");
    //Serial.print("  INTFA - ");Serial.println(mcp.readRegister(MCP23017_INTFA), BIN);
    //Serial.print("  INTFB - ");Serial.println(mcp.readRegister(MCP23017_INTFB), BIN);
    //Serial.print("  GPIOA - ");Serial.println(mcp.readRegister(MCP23017_GPIOA), BIN);
    //Serial.print("  GPIOB - ");Serial.println(mcp.readRegister(MCP23017_GPIOB), BIN);
    //configMCP();
    //Serial.println("---------------------------------------------");
  }*/

  //logging.debug("INTCAPA " + String(mcp.readRegister(MCP23017_INTCAPA)));
  //logging.debug("INTCAPB " + String(mcp.readRegister(MCP23017_INTCAPB)));

}

//...............................................................................
// REGISTER
//...............................................................................
/*
IODIRx [RW] Datenrichtungsregister der GPIO-Ports:
  1 = INPUT; 0 = OUTPUT
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_IODIRB, B11111111);

IPOLx [RW] Polarität
  1 = invertiert; 0 = nicht invertiert
  mcpGPIO.mcp.writeRegister(MCP23017_IPOLA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_IPOLB, B11111111);

GPINTENx [RW] Interrupt-On-Change-Funktion
  1 = IRQ enabled; 0 = IRQ disabled
  Es müssen zusätzlich die DEFVAL- und INTCON-Register konfiguriert werden.
  mcpGPIO.mcp.writeRegister(MCP23017_GPINTENA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_GPINTENB, B11111111);

DEFVALx [RW] IRQ-Vergleichsregister
  Vergleich GPIO mit DEFVALx bei Opposition und aktivem IRQ über GPINTEN und INTCON wird ein Interrupt ausgelöst.
  mcpGPIO.mcp.writeRegister(MCP23017_DEFVALA, B00000000);
  mcpGPIO.mcp.writeRegister(MCP23017_DEFVALB, B00000000);

INTCONx [RW] Interruptmode
  1 = vergleich mit DEFVALx; 0 = Interrupt-On-Change
  mcpGPIO.mcp.writeRegister(MCP23017_INTCONA, B00000000);
  mcpGPIO.mcp.writeRegister(MCP23017_INTCONB, B00000000);

GPPUx [RW] INPUT Pull-Up 100k
  1 = enabled, 0 = disabled
  mcpGPIO.mcp.writeRegister(MCP23017_GPPUA, B11111111);
  mcpGPIO.mcp.writeRegister(MCP23017_GPPUB, B11111111);

INTFx [R] Interrupt detected
  1 = IRQ detected; 0 = no IRQ detected

 INTCAPx [R] GPIO-Interrupt-State
 Das Register kann nur gelesen werden und es wird nur beim Auftreten eines Interrupts aktualisiert.
 Die Registerwerte bleiben unverändert, bis der Interrupt über das Lesen von INTCAP oder GPIO gelöscht.

GPIOx [RW] GPIO-Portregister
  Ein Lesen der Register entspricht dem Lesen der Portleitungen.
  Ein Schreiben in diese Register entspricht dem Schreiben in die Ausgangs-Latches (OLAT).

OLATx [RW] Outputregister
  Ein Lesen von diesen Registern liefert den zuletzt hinein geschriebenen Wert, nicht die an den Pins aliegenden Daten.
  Ein Schreiben auf diese Register ändert die Ausgangs-Latches, die ihrerseits diejenigen Pins beeinflussen,
  die als Ausgänge konfiguriert sind.

IOCON [RW] IO-Konfigurationsregister
  mcpGPIO.mcp.writeRegister(MCP23017_IOCONA, B01000000);

  IOCON.7 Bank-Konfiguration
   Ist BANK = 1, werden die Register nach Ports getrennt.
   Die Register von PORT A belegen die Adressen 0x00 bis 0x0A und die Register von PORT B die Adressen
   von 0x10 bis 0x1A (linke Spalte der Tabelle).
   Ist BANK = 0, werden die A- und B-Register gepaart.
   Zum Beispiel hat IODIRA die Adresse 0x00 und IODIRB die darauf folgende Adresse 0x01 (rechte Spalte der Tabelle).

   BANK = 0
   Schreiben von 0x80 in die Adresse 0x0A (ICON), um das BANK-Bit auf "1" zu setzen.
   Nach Abschluß des Schreibvorgangs zeigt der interne Adressepointer nun auf die folgende Adresse 0x0B, die ungültig ist.

  IOCON.6 MIRROR-Bit
   1 = MIRROR INTA & INTB; 0 = INTA und INTB separat
  IOCON.5 SEQOP AUTO increment Address-Pointer
   1 = disabled; 0 = enabled

  IOCON.4 DISSLW-Bit (Slew-Rate) auf der SDA-Leitung
   1 = nicht beeinflusst; 0 = beeinflusst

  IOCON.3 HAEN-Bit Hardware-Adressierung bei SPI (MCP23S17 only)

 IOCON.2 ODR-Bit INT-Pin-Open-Drain-Konfiguration
   1 = enabled; 0 = disabled Polarität wird von INTPOL-Bit festgelegt

  IOCON.1 INTPOL-Bit INT-Pin-Polarität (wenn ODR-Bit = 0)
   1 = high-active; 0 = low-active

  IOCON.0 allways 0
*/
