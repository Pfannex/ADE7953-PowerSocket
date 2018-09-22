#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"
#include <FunctionalInterrupt.h>
#include "Adafruit_MCP23017.h"

//###############################################################################
//  GPIO
//###############################################################################

#define MCP23017_Name    "module::MCP23017"
#define MCP23017_Version "0.1.0"

//###############################################################################
//  GPIO
//###############################################################################
// time in ms pin must be stable before reporting change
#define MCPDEBOUNCETIME 2000

class MCP23017 : public Module {

public:
  MCP23017(string name, LOGGING &logging, TopicQueue &topicQueue,
           int GPIOIrqPin, int sda, int scl);
  int irqPin;
  int sda;
  int scl;

  void start();
  void handle();
  String getVersion();

  Adafruit_MCP23017 mcp;

private:
  void clearIRQ();
  void irq();                         // irq jumpTo funktion
  void irqHandle();                   // handle irq function
  #define irqOFF 4                    // irq detachInterrupt mode
  void irqSetMode(int mode);          // irq mode setter
  volatile bool irqDetected = false;
  unsigned long lastIrqTime = 0;
};

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
