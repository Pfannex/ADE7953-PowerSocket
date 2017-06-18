#pragma once
  #include <Arduino.h>
  #include "SysUtils.h"
  #include "Hash.h"
  #include "Logging.h"

//###############################################################################
//  Authentification 
//###############################################################################

class Auth{
public:
    Auth();
	SysUtils sysUtils;
	LOGGING logging;
	
    void reset();
    bool checkPassword(String username, String password);
    String createSession(String username);
    bool checkSession(String SessionID);

private:
    long seed= 0;
    long rand(long max);
};
