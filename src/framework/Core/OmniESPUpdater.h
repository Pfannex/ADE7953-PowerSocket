#pragma once

#include <FS.h>
#include <Arduino.h>
#include <Updater.h>
#include "framework/Utils/Logger.h"


#define DEFAULTTARBALL "omniesp"
#define UPDATEFOLDER "/update/"
#define FIRMWAREBIN "firmware/firmware.bin"
#define DEVICECONFIG "customDevice/config.json"

enum Header {
	NAME = 0, MODE = 100, UID = 108, GID = 116, SIZE = 124,
	CHK = 148, TYPE = 156, LINK = 157,  END = 512
};

#define TARBLOCKSIZE 512
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))


enum Type {
	REG = '0', DIRECTORY = '5'
};

class Tarball {

public:
	Tarball(const char* deviceName);
	String getFilename();
	bool exists();
	bool remove();
	bool beginWrite();
	size_t position();
	bool write(uint8_t *data, size_t len) ;
	bool endWrite();
	bool beginRead();
	size_t read(uint8_t *data, size_t len) ;
	bool endRead();
	String getLastError();
private:
	String filename;
	File file;
	bool reading= false;
	bool writing= false;
	void setErrorMsg(String msg);
	String errorMsg= "";
};

class OmniESPUpdater {

public:
  OmniESPUpdater(LOGGING& logging);
	String getLastError();
	bool doUpdate(const char* deviceName, bool setDeviceDefaults);


private:
  LOGGING& logging;
	String getUpdateErrorString(uint8_t error);
	bool untar(Tarball& tarball);
  void chksum(const char b[END], char *chk);
  bool extract(Tarball& tarball, char *fname, int l, char b[END]);
	void setErrorMsg(String msg);
	bool extractFiles(Tarball tarball, bool setDeviceDefaults);
	bool flash(String filename);
	void listSpiffs();
	String errorMsg= "";
	bool skipConfigFiles;
};
