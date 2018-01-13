#pragma once
  #include <Arduino.h>
  #include "Logger.h"
  #include "Hash.h"
  #include "API.h"

//###############################################################################
//  Session
//###############################################################################

#define sessionLifetime 600000 // session life time in milliseconds (10 minutes)

class Session {

public:
   Session(String username, API& api);
   String getSessionId();
   String getUsername();
   bool isExpired();
   void touch();


 private:
   API& api;
   String username;
   String sessionId;
   long lastTouch;

};

typedef Session* SessionPtr;

//###############################################################################
//  Authentification
//###############################################################################

#define maxSessions 10 // maximum number of simultaneous connections

class Auth {

  public:

    // constructor
    Auth(API& api);

    // destructor
    ~Auth();

    // forget all sessions
    void reset();

    // check if username/password match an entry in the database
    bool checkPassword(String username, String password);
    // creates a new session for a user named username
    SessionPtr createSession(String username);
    // gets the session with a given ID, returns NULL if none exists
    SessionPtr getSession(String sessionId);
    // removes a session
    void deleteSession(String sessionId);

  private:
    API& api;
    int numSessions= 0;
    SessionPtr* sessions;


};
