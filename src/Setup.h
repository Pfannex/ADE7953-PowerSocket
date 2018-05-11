//###############################################################################
//  Version
//###############################################################################
#define VERSION "OmniESP 0.8.0"

//###############################################################################
// Debugging
//###############################################################################

#define DEBUG 1
#ifdef DEBUG
  #define DEBUG_UPDATER Serial
#endif

//###############################################################################
//  User
//###############################################################################

// defaults for username and password
#define USERNAME "admin"
#define PASSWORD "admin"

//###############################################################################
//  Web
//###############################################################################
#define TITLE "OmniESP"
#define WEBSERVERPORT 80

//###############################################################################
//  NTP
//###############################################################################

// default configuration if no other is set in the config
#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_UPDATE_INTERVAL 3600000
#define NO_TIME_OFFSET 0
#define TZ_CET

//###############################################################################
//  FlashFileSystem
//###############################################################################

#define CFG_PATH        "/OmniESP.json"              // global configuration
#define DEVICECFG_PATH  "/customDevice/config.json"  // device configuration
#define DASHBOARD_PATH  "/customDevice/dashboard.json"

#define TYPE_OBJECT 1
#define TYPE_ARRAY 0
