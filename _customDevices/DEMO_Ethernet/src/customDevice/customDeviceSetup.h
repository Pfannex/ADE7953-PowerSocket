//###############################################################################
// ESP8266
//###############################################################################
/*
GPIO WeMos ESPpin Function  Connection
 2   D4    17               PullUp
 0   D3    18               PullUp
 4   D2    19     SDA
 5   D1    20     SCL
 3   RX    21
 1   TX    22
15   D8    16     SS        PullDown
13   D7     7     MOSI
12   D6     6     MISO
14   D5     5     SCK
16   D0     4     SLEEP!
ADC  A0     2     Analog
*/


//###############################################################################
// GPIO
//###############################################################################

/*
 * CLK  = GPIO 14 | D5
 * MISO = GPIO 12 | D6
 * MOSI = GPIO 13 | D7
 * CS   = GPIO 15 | D8   PNP-Transistor [B]3k3-GPIO15 | [E]-CS | [C]-GND

 //WARNING
 //W5500 CS has internal PullUp, GPIO15 is also used for BootMode-selection
 //with GPIO15 set to HIGH ESP starts with wrong BootMode!
 //set CS-pin to GPIO4 or use PNP-Transistor,

 https://esp8266hints.wordpress.com/category/hardware-2/w5500/
 */



// Arduino Pin 12 = = IO12 = Physical Pin 6 = NodeMCU/WeMos Pin D6
//#define PIN_BUTTON 12

// Arduino Pin 15 = IO15 = Physical Pin 16 = NodeMCU/WeMos Pin D8
//#define PIN_LED 15

// Arduino Pin 14 = IO14 = Physical Pin 5 = NodeMCU/WeMos Pin D5
//#define PIN_RELAY 14

// Arduino Pin 13 = IO13 = Physical Pin 7 = NodeMCU/WeMos Pin D7
//#define OWPIN 13
