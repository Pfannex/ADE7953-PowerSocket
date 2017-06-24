#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "Hash.h"

//###############################################################################
//  Authentification 
//###############################################################################

class Auth{
public:
    SysUtils sysUtils;

    Auth();
    void reset();
    bool checkPassword(String username, String password);
    String createSession(String username);
    bool checkSession(String SessionID);

private:
    long seed= 0;
    long rand(long max);
};

