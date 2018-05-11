#pragma once

#define QUOTEME(M)       #M

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

#define DEVICETYPE      "SimpleSwitch"
#define DEVICEVERSION   "1.1.0"

#define DEVICE           simpleSwitch
#define INCLUDE_FILE(M)  QUOTEME(device/_devices/simpleSwitch.h)
#define DASHBOARD_PATH   "/dashboards/simpleSwitch.json"
#define DEVICECFG_PATH   "/cfg/device/simpleSwitch.json"

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
#define INCLUDE_FILE(M)  QUOTEME(device/_examples/DEMO_GPIO.h)
#define DASHBOARD_PATH   "/dashboards/DEMO_GPIO.json"
#define DEVICECFG_PATH   "/cfg/examples/DEMO_GPIO.json"
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

/*
#define DEVICETYPE      "DEMO_I2C_OW"
#define DEVICEVERSION   "1.0.0"
#define DEVICE           DEMO_I2C_OW
#define INCLUDE_FILE(M)  QUOTEME(device/_examples/DEMO_I2C_OW.h)
#define DASHBOARD_PATH   "/dashboards/DEMO_I2C_OW.json"
#define DEVICECFG_PATH   "/cfg/examples/DEMO_I2C_OW.json"
*/
