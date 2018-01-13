//###############################################################################
//  Version
//###############################################################################
#define VERSION "OmniESP 0.8.0"

//###############################################################################
// Debugging
//###############################################################################

#define DEBUG 0

//###############################################################################
//  Web
//###############################################################################
#define TITLE "OmniESP"
#define WEBSERVERPORT 80

//###############################################################################
//  NTP
//###############################################################################

// default server if no other is set in the config
#define NTP_SERVER "europe.pool.ntp.org"
#define NTP_UPDATE_INTERVAL 3600000
#define NO_TIME_OFFSET 0
#define TZ_CET

//###############################################################################
//  FlashFileSystem
//###############################################################################
#define CFG_PATH        "/cfg.json"           //global configuration
#define DEVICECFG_PATH  "/deviceCFG.json"     //device specific configuration
#define WEBCFG_PATH     "/webCFG.json"        //WebIF specific configuration
#define SUBGLOBAL_PATH  "/subGlobal.json"     //global MQTT subscribe

#define TYPE_OBJECT 1
#define TYPE_ARRAY 0
