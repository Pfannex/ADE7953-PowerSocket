#include <FS.h>
#include <Arduino.h>
#include <Updater.h>
#include "framework/Utils/Logger.h"



#define UPDATEFOLDER "/update/"

enum Header {
	NAME = 0, MODE = 100, UID = 108, GID = 116, SIZE = 124,
	CHK = 148, TYPE = 156, LINK = 157,  END = 512
};

#define TARBLOCKSIZE 512

enum Type {
	REG = '0', DIRECTORY = '5'
};


class OmniESPUpdater {

public:
  OmniESPUpdater(LOGGING& logging);
  bool hasUpdate(String deviceName);
	String getLastError();
	bool doUpdate(String deviceName);

private:
  LOGGING& logging;
  String getFilename(String deviceName);
  bool untar(File& tarBall);
  void chksum(const char b[END], char *chk);
  bool extract(File& tarBall, char *fname, int l, char b[END]);
	void setErrorMsg(String msg);
	String errorMsg= "";
};
