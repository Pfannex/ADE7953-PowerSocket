//###############################################################################
//  Version
//###############################################################################
  #define VERSION "OmniESP 0.8.0"

//###############################################################################
//  Web
//###############################################################################
 #define TITLE "OmniESP"
 #define WEBSERVERPORT 80

//###############################################################################
//  NTP
//###############################################################################

  // default server if no other is set in the config
  #define NTP_SERVER  "europe.pool.ntp.org"
  #define NTP_UPDATE_INTERVAL 3600000
  #define NO_TIME_OFFSET 0
  #define TZ_CET

//###############################################################################
//  FlashFileSystem
//###############################################################################
#ifndef __FFS_PATH__
#define __FFS_PATH__

  #define CFG_PATH        "/config.json"
  #define SUB_PATH        "/sub.json"
  #define SUB_GLOBAL_PATH "/subGlobal.json"
  #define PUB_PATH        "/pub.json"
  #define ADE7953_PATH    "/ADE7953.json"

  #define TYPE_OBJECT 1
  #define TYPE_ARRAY  0
#endif

//###############################################################################
//  GPIO
//###############################################################################
//I2C
  #define I2C_SDA 4
  #define I2C_SCL 5

  #define HAS_LED
  //#define HAS_NEOPIXEL
  #define HAS_BUTTON
  #define HAS_RELAY


//###############################################################################
//  I2C slave addresses
//###############################################################################

  #define SSD1306_I2C_adr 0x3c
