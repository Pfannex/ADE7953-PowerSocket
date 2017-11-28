#include "WebServer.h"

//###############################################################################
//  web interface
//###############################################################################

WebServer::WebServer(API &api)
    : api(api), webServer(WEBSERVERPORT), webSocket("/ws"), auth(api) {}
