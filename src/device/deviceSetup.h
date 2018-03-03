//###############################################################################
// SimpleSwitch
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

//...............................................................................
// DEVICE
//...............................................................................
/*
#define DEVICETYPE      "SimpleSwitch"
#define DEVICEVERSION   "1.1.0"

#define DEVICE           simpleSwitch
#define QUOTEME(M)       #M
#define INCLUDE_FILE(M)  QUOTEME(device/_devices/simpleSwitch.h)
*/

//###############################################################################
// DEMO GPIO
//###############################################################################

//
// This device is a .....
//
//
//
//
// API:
// ~/set/device/power 0|1
// ~/get/device/power

//...............................................................................
// DEVICE
//...............................................................................

/*
#define DEVICETYPE      "DEMO_GPIO"
#define DEVICEVERSION   "1.0.0"

#define DEVICE           DEMO_GPIO
#define QUOTEME(M)       #M
#define INCLUDE_FILE(M)  QUOTEME(device/_examples/DEMO_GPIO.h)
*/
//###############################################################################
// DEMO I2C
//###############################################################################

//
// This device is a .....
//
//
//
//
// API:
// ~/set/device/power 0|1
// ~/get/device/power

//...............................................................................
// DEVICE
//...............................................................................


#define DEVICETYPE      "DEMO_I2C"
#define DEVICEVERSION   "1.0.0"

#define DEVICE           DEMO_I2C
#define QUOTEME(M)       #M
#define INCLUDE_FILE(M)  QUOTEME(device/_examples/DEMO_I2C.h)
