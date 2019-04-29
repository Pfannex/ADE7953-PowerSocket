//###############################################################################
//  Version
//###############################################################################

// we use no explicit versioning, it's all derived from the git versioning

#define VERSION "OmniESP"

//###############################################################################
//  User
//###############################################################################

// defaults for username and password
#define USERNAME "admin"
#define PASSWORD "admin"
#define APPASSWORD "password"

//###############################################################################
//  Web
//###############################################################################
#define TITLE "OmniESP"
#define WEBSERVERPORT 80

//###############################################################################
//  NTP and Time
//###############################################################################

// default configuration if no other is set in the config
#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_UPDATE_INTERVAL 3600000
#define NO_TIME_OFFSET 0
#define TZ_CET
// all timestamps from device in ms if defined
#define TIMESTAMP_MS

//###############################################################################
//  FlashFileSystem
//###############################################################################

#define VERS_PATH       "/version.json"              // version
#define CFG_PATH        "/OmniESP.json"              // global configuration
#define DEVICECFG_PATH  "/customDevice/config.json"  // device configuration
#define DASHBOARD_PATH  "/customDevice/dashboard.json"

#define TYPE_OBJECT 1
#define TYPE_ARRAY 0

//###############################################################################
//  Other
//###############################################################################

// try at least every 5 minutes
#define RECONNECT_DELAY_MAX (300)
