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

// Arduino Pin 12 = = IO12 = Physical Pin 6 = NodeMCU/WeMos Pin D6
#define PIN_BUTTON 12
// Arduino Pin 15 = IO15 = Physical Pin 16 = NodeMCU/WeMos Pin D8
#define PIN_LED 15
// Arduino Pin 14 = IO14 = Physical Pin 5 = NodeMCU/WeMos Pin D5
#define PIN_RELAY 14
#define PIN_QRE 13
#define PIN_WS2812 5
#define LEDSCOUNT 3
