/******************************************************************************

  ProjectName: ESP8266_Basic                      ***** *****
  SubTitle   : ESP8266 Template                  *     *     ************
                                                *   **   **   *           *
  Copyright by Pf@nne                          *   *   *   *   *   ****    *
                                               *   *       *   *   *   *   *
  Last modification by:                        *   *       *   *   ****    *
  - Pf@nne (pf@nne-mail.de)                     *   *     *****           *
                                                 *   *        *   *******
  Date    : 29.03.2016                            *****      *   *
  Version : alpha 0.200                                     *   *
  Revison :                                                *****

********************************************************************************/
#pragma once
#define Version "ESP_Basic alpha V0.200"
 
  typedef char* topicField; 

//publish struct
  const int pub_e1 = 4;			//define TreeDepht here!!!
  const int pub_e2 = 5;
  const int pub_e3 = 30;
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
  const int sub_e1 = 4;			//define TreeDepht here!!!
  const int sub_e2 = 5;
  const int sub_e3 = 4;
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
    char version[25] = Version;
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
	char updateServer[20];
	char filePath[40];
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
  
// MyFile struct
  typedef struct MyFile{
    char Field_01[15];
    char Field_02[15];
  };
  
 
  