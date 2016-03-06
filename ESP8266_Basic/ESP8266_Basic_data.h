/******************************************************************************

  ProjectName: ESP8266 Basic                      ***** *****
  SubTitle   : Data structs                      *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 04.03.2016                            *****      *   *
  Version : alpha 0.102                                     *   *
  Revison :                                                *****

********************************************************************************/
#pragma once
 
  typedef char* topicField; 

//publish struct
  const int pub_e1 = 2;			//define TreeDepht here!!!
  const int pub_e2 = 4;
  const int pub_e3 = 3;
  const int pub_e4 = 0;
  
  typedef struct Tpub_topicE1{
    topicField item[pub_e1];
	int count;
  };
  typedef struct Tpub_topicE2{
    topicField item[pub_e1][pub_e2];
	int count;
  };
  typedef struct Tpub_topicE3{
    topicField item[pub_e1][pub_e2][pub_e3];
	int count;
  };
 
  typedef struct Tpub_Topic{
    Tpub_topicE1 E1;
    Tpub_topicE2 E2;
    Tpub_topicE3 E3;	
  };
  
//subscribe struct
  const int sub_e1 = 2;			//define TreeDepht here!!!
  const int sub_e2 = 4;
  const int sub_e3 = 3;
  const int sub_e4 = 0;
  
  typedef struct Tsub_topicE1{
    topicField item[sub_e1];
	int count;
  };
  typedef struct Tsub_topicE2{
    topicField item[sub_e1][sub_e2];
	int count;
  };
  typedef struct Tsub_topicE3{
    topicField item[sub_e1][sub_e2][sub_e3];
	int count;
  };
 
  typedef struct Tsub_Topic{
    Tsub_topicE1 E1;
    Tsub_topicE2 E2;
    Tsub_topicE3 E3;	
  };
  
 
class Topics{
public:
  Topics();
  Tpub_Topic pub; 
  Tsub_Topic sub; 
private:
}; 


// Config struct
  typedef struct CFG{
    char version[15] = "alpha V0.102";
    char webUser[40];
    char webPassword[40];
    char apName[40];
    char apPassword[40];
    char wifiSSID[40];
    char wifiPSK[40];
    char wifiIP[20];
    char mqttServer[20];
    char mqttPort[6];
    char mqttDeviceName[20];
	char mqttStatus[20];
  };
  
  typedef struct TdissectResult{
    bool found = false;
    String topic = "";
	String value = "";
	String itemPath = "";
	int treeDepth = 0;
    int E1 = NULL;
    int E2 = NULL;
    int E3 = NULL;
    int E4 = NULL;   
  };
  
 
  