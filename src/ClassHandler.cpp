#include "ClassHandler.h"

//###############################################################################
//  ClassHandler
//###############################################################################
ClassHandler::ClassHandler():
    api(sysUtils, ffs, wifi, mqtt, webif, i2c, oWire),
    ffs(sysUtils),
    mqtt(sysUtils, wifi),
    wifi(sysUtils),
    webif(sysUtils){
}
