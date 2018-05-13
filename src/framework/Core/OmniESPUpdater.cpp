#include "framework/Core/OmniESPUpdater.h"
#include "framework/Utils/Debug.h"

// auto-update weglassen

// update device with content of file

// check SHA256
// see
// https://github.com/CSSHL/ESP8266-Arduino-cryptolibs/blob/master/sha256/examples/sha256-test/sha256-test.ino

// unpack

/*
  cfg/device/<name>.json (optional)
  dashboards/<name>.json
  web/**
*/

// flash firmware`



// constructor
OmniESPUpdater::OmniESPUpdater(LOGGING &logging) : logging(logging){};

void OmniESPUpdater::setErrorMsg(String msg) {
  logging.error(msg);
  errorMsg= msg;
}

String OmniESPUpdater::getLastError() {
  return errorMsg;
}


String OmniESPUpdater::getFilename(String deviceName) {
  return UPDATEFOLDER+deviceName+".tar";
}

bool OmniESPUpdater::hasUpdate(String deviceName) {
  return SPIFFS.exists(getFilename(deviceName));
}

void OmniESPUpdater::chksum(const char b[END], char *chk) {
	unsigned sum = 0, i;
	for(i = 0; i<END; i++)
		sum += (i >= CHK && i < CHK+8) ? ' ' : b[i];
	snprintf(chk, 9, "0%5o", sum);
}

bool OmniESPUpdater::extract(File& tarBall, char *fname, int l, char b[END]) {
	static char chk[9] = {0};
	int r = 0;
	FILE *f = NULL;
  String filename= fname;
  logging.info("extracting "+filename);

	//unlink(fname);
	switch(b[TYPE]) {
	case REG:
		/*r = !(f = fopen(fname, "w")) ||
			chmod(fname,strtoul(b + MODE, 0, 8));*/
      D("regular file");
		break;
	case DIRECTORY:
    D("directory");
    // nothing to do, FFS does not support directories
		break;
	default:
		setErrorMsg(filename+": unsupported filetype");
    return false;
	}
	chksum(b, chk);
	Ds(b+CHK, chk);
	if(strncmp(b+CHK, chk, 6)) {
    setErrorMsg(filename+": wrong checksum");
    return false;
  }
  char x[11]= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int br= 0, bw= 0;
	for(; l > 0; l -= TARBLOCKSIZE) { // block is always TARBLOCKSIZE
    Di("reading from position:", tarBall.position());
		int k= tarBall.readBytes(b, TARBLOCKSIZE);
    br+= k;
    Di("bytes read from tarBall:", k);
    strncpy(x, b, 10); Ds("begin of block:", x);
    #define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
    bw+= MIN(l, TARBLOCKSIZE);
		//if() fwrite(b, MIN(l, TARBLOCKSIZE), 1, f);
	}
  Di("bytes read:", br);
  Di("bytes written:", bw);
	//if(f) fclose(f);
	return true;
};

bool OmniESPUpdater::untar(File& tarBall) {
  int l;
  char b[END], fname[101] = { 0 };

  char x[11]= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  logging.info("extracting files from tarball");
  int k;
  while(k= tarBall.readBytes(b, END)) {
    Di("main loop, bytes read from tarBall:", k);
    Di("now at position:", tarBall.position());
    strncpy(x, b, 10); Ds("begin of block:", x);
    if(*b == '\0')
      break;
    memcpy(fname, b, 100);
    Ds("filename is:", fname);
    l = strtol(b+SIZE, 0, 8);
    Di("size is:" , l)
    if(!extract(tarBall, fname, l, b)) return false;
    Di("now at position:", tarBall.position());
  }
  return true;
}

bool OmniESPUpdater::doUpdate(String deviceName) {

  String fileName= getFilename(deviceName);
  if(!hasUpdate(deviceName)) {
    setErrorMsg("tarball "+fileName+" not found");
    return false;
  }
  logging.info("starting update");
  File tarBall= SPIFFS.open(fileName, "r");
  if(!tarBall) {
    setErrorMsg("could not open tarball "+fileName);
    return false;
  }
  logging.info("using tarball "+fileName);
  untar(tarBall);
  tarBall.close();
  logging.info("update finished");
}
