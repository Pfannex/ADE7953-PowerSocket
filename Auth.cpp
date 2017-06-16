//###############################################################################
//  Authentification 
//###############################################################################

#include "Auth.h"
#include "Hash.h"
#include "Logging.h"

Auth::Auth() {

}

long Auth::rand(long max) {
 
  // We initialize the random number generator when it is first used.
  // Thus a random number of milliseconds has passed when we come here.
  if(!seed) {
      seed= millis();
      randomSeed(seed);
  }
  return random(max);
}

void Auth::reset() {
  
}
 
bool Auth::checkPassword(String username, String password) {

  String sha1hash= sha1(password);
  info("authentificating user "+username+" with SHA1 hash "+sha1hash+"... ");
  
  // this is a stub!
  return (username == "admin");
  
}


String Auth::createSession(String username) {

  // the session id should be as random as possible 
  String SessionID= "";
  for(int i= 0; i< 32; i++) {
    SessionID.concat(String(rand(256), HEX));
  }
  // add session to list
  // ...
  
  info("session " + SessionID + " created for user " + username);
  return SessionID;

}    

bool Auth::checkSession(String SessionID) {
 
  // check if we have a session with this ID
  // ...
  info("checking session " + SessionID);
  return (SessionID != "0");
}
