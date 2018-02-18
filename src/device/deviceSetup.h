//###############################################################################
// DESCRIPTION
//###############################################################################

//
// This device is a simple switch.
// A short button press toggles a relay. The LED indicates the state (on/off).
// A long click turns the config mode and makes the LED blink. The config mode
// is left after some inactivity.
//
// API:
// ~/set/device/power 0|1
// ~/get/device/power

//###############################################################################
// DEVICE
//###############################################################################

#define DEVICETYPE      "_Template"
#define DEVICEVERSION   "1.0.0"
#define DEVICE           DEMO_GPIO
#define QUOTEME(M)       #M
#define INCLUDE_FILE(M)  QUOTEME(device/_examples/DEMO_GPIO.h)



//###############################################################################
// GPIO
//###############################################################################

// Arduino Pin 12 = = IO12 = Physical Pin 6 = NodeMCU/WeMos Pin D6
#define PIN_BUTTON 12

// Arduino Pin 15 = IO15 = Physical Pin 16 = NodeMCU/WeMos Pin D8
#define PIN_LED 15

// Arduino Pin 14 = IO14 = Physical Pin 5 = NodeMCU/WeMos Pin D5
#define PIN_RELAY 14
