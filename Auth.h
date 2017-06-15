
#include <Arduino.h>

//###############################################################################
//  Authentification 
//###############################################################################

class Auth {
  
private:
    long seed= 0;
    long rand(long max);
    
public:
    Auth();
    void reset();
    bool checkPassword(String username, String password);
    String createSession(String username);
    bool checkSession(String SessionID);

};
