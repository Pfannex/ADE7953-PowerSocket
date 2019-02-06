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

#define WS_PIN  5
#define QRE_PIN 4
#define S0 14
#define S1 12
#define S2 13
#define S3 15
#define CHANNELSCOUNT 16

#define LEDCOUNT 3

// threshold to detect ON
#define THRESHOLD_ON 500
// threshold to detect OFF
#define THRESHOLD_OFF 500
// timeout maximum RC-loading time
#define TIMEOUT 3000
// time in ms pin must be stable before reporting change
#define DEBOUNCETIME 100  //50
