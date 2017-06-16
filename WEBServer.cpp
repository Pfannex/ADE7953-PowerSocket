#include "WEBServer.h"
#include "FS.h"
#include "Hash.h"

//###############################################################################
//  web interface 
//###############################################################################

WEBIF::WEBIF(FFS& ffs): webServer(80), ffs(ffs) { 

  httpUpdater.setup(&webServer); 

  // static pages
  // we need short file names on SPIFFS, else they do not load
  webServer.serveStatic("/lib/jquery.js", SPIFFS, "/web/lib/jquery.js", "max-age=86400");
  webServer.serveStatic("/lib/jquery.mobile.js", SPIFFS, "/web/lib/jquery.mobile.js", "max-age=86400");
  webServer.serveStatic("/css/jquery.mobile.css", SPIFFS, "/web/css/jquery.mobile.css", "max-age=86400");  
  webServer.serveStatic("/auth.js", SPIFFS, "/web/auth.js", "max-age=86400");  
  webServer.serveStatic("/css/images/ajax-loader.gif", SPIFFS, "/web/css/images/ajax-loader.gif", "max-age=86400");  
  // dynamic pages
  webServer.on("/", std::bind(&WEBIF::rootPageHandler, this));
  webServer.on("/auth.html", std::bind(&WEBIF::authPageHandler, this));
  webServer.onNotFound(std::bind(&WEBIF::handleNotFound, this));
  
}

//-------------------------------------------------------------------------------
//  web interface public
//-------------------------------------------------------------------------------

//...............................................................................
//  web interface start
//...............................................................................
void WEBIF::start() {
  
  Serial.print("Web interface... ");
  
  // pages served
  numPagesServed= 0;

  // Authentificator
  auth.reset();
  
  //here the list of headers to be recorded
  const char* headerkeys[] = {"User-Agent","Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
  
  //ask server to track these headers
  webServer.collectHeaders(headerkeys, headerkeyssize );  
  
  // start serving requests
  webServer.begin();
  Serial.println(" started");

}

//...............................................................................
//  web interface handle
//...............................................................................

void WEBIF::handle() {
  
   webServer.handleClient();
}

//-------------------------------------------------------------------------------
//  web interface private
//-------------------------------------------------------------------------------

String WEBIF::subst(String data) {

  /*
  int p1= 0, p2;
  String key;
  
  while((p1= data.indexOf('$DATA('), p1)>= 0) {
    p2= data.indexOf(')', p1+6);
    key= data.substring(p1+6,p2-1);
    if(key == 'MACADDRESS') {
        data= data.substring(0,p1
    }
  }*/
  data.replace("$DATA(MACADDRESS)", "FF:FF:FF:FF:FF:FF");
  return data;
  
}


void WEBIF::send(const String &description, int code, char *content_type, const String &content) {
 
  numPagesServed++;
  Serial.println("Serving "+description);
  webServer.send(code, content_type, content);
}

void WEBIF::sendFile(const String &description, int code, char *content_type, const String filePath) {
    
  
  // http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html
  // https://github.com/pellepl/spiffs/wiki/Using-spiffs
  File f;
  
  Serial.println("Serving "+description);
  Serial.print("reading " + filePath + "... ");
  if (SPIFFS.exists(filePath)) {
    f = SPIFFS.open(filePath, "r");
    if (f) {
      Serial.println("OK");
      webServer.send(code, content_type, subst(f.readString()));
      f.close();
    } else {
      Serial.println("ERROR (open)");
    }
  } else {
      Serial.println("ERROR (no such file)");
  }  

}

//###############################################################################
//  page handlers
//###############################################################################

void WEBIF::rootPageHandler() {
  
  Serial.println("Serving root page...");
  
  // Debug only
  //Serial.println("Headers:");
  //for(int i= 0; i< webServer.headers(); Serial.println(webServer.header(i++)));
  
  if(webServer.hasHeader("User-Agent")) {
    Serial.println("User-Agent: "+webServer.header("User-Agent"));
  }
  
  bool authenticated= false;

  // check for cookie
  if(webServer.hasHeader("Cookie")) {
    String cookie = webServer.header("Cookie");
    Serial.println("Client provided cookie: "+cookie);
    // check if cookie corresponds to active session
    authenticated= auth.checkSession(cookie.substring(10));
  } else {
    authenticated= false;
  }
  
  if(authenticated) {
    // show dashboard
    Serial.println("Request authenticated.");
    sendFile("dashboard", 200, "text/html;charset=UTF-8", "/web/ui.html");
  } else {
    // send user to login page  
    Serial.println("Request not authenticated.");
    sendFile("login page", 200, "text/html;charset=UTF-8", "/web/login.html");
  }
}

void WEBIF::authPageHandler() {
 
  String action= webServer.arg("action");

  if(action == "login") {  

    // --- login
    String username= webServer.arg("username");
    String password= webServer.arg("password");
    
    if(auth.checkPassword(username, password)) {
      // password ok
      Serial.println("OK");
      String SessionID= auth.createSession(username);
      webServer.sendHeader("Location","/");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.sendHeader("Set-Cookie","SessionID="+SessionID);
      webServer.send(200, "text/plain", "true");
      
    } else {
      // password not ok
      Serial.println("Failed");
      webServer.sendHeader("Location","/");
      webServer.sendHeader("Cache-Control","no-cache");
      webServer.send(200, "text/plain", "false");
      
    }
    
  } else if(action == "logout") {
    
    // --- logout
    webServer.sendHeader("Location","/");
    webServer.sendHeader("Cache-Control","no-cache");
    webServer.sendHeader("Set-Cookie","SessionID=0");
    webServer.send(200, "text/plain", "true");
  }
  
}

//...............................................................................
//  404 file not found
//...............................................................................

void WEBIF::handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  
  for (uint8_t i = 0; i < webServer.args(); i++){
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
}



