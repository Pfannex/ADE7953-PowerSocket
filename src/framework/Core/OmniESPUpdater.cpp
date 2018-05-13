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

//###############################################################################
//  Tarball
//###############################################################################

//...............................................................................
//  constructor
//...............................................................................

Tarball::Tarball(const char *deviceName) {
  filename = UPDATEFOLDER + String(deviceName) + ".tar";
}

//...............................................................................
//  error message handling
//...............................................................................

void Tarball::setErrorMsg(String msg) { errorMsg = msg; }

String Tarball::getLastError() { return errorMsg; }

//...............................................................................
//  tarball handling
//...............................................................................

String Tarball::getFilename() { return filename; }

bool Tarball::exists() { return SPIFFS.exists(getFilename()); }

bool Tarball::remove() {
  Ds("tarball removing...", filename.c_str());
  if (!SPIFFS.exists(filename))
    return true;
  D("tarball do actual remove on SPIFFS");
  if (SPIFFS.remove(filename)) {
    return true;
  } else {
    D("error removing tarball");
    setErrorMsg("could not delete tarball " + filename);
    return false;
  }
}

bool Tarball::beginWrite() {
  if (writing)
    endWrite();
  if (reading)
    endRead();
  D("tarball begin write");
  file = SPIFFS.open(filename, "w");
  if (!file) {
    setErrorMsg("could not create tarball " + filename);
  } else {
    writing = true;
  }
  return writing;
}

bool Tarball::write(uint8_t *data, size_t len) {
  Di("tarball write bytes:", len);
  if (file.write(data, len) == len)
    return true;
  else {
    setErrorMsg("short write");
    return false;
  }
}

bool Tarball::endWrite() {
  D("tarball end write");
  file.flush();
  file.close();
  writing = false;
}

bool Tarball::beginRead() {
  if (writing)
    endWrite();
  if (reading)
    endRead();
  file = SPIFFS.open(filename, "r");
  if (!file) {
    setErrorMsg("could not open tarball " + filename);
  } else {
    reading = true;
  }
  return reading;
}

size_t Tarball::read(uint8_t *data, size_t len) { return file.read(data, len); }

bool Tarball::endRead() {
  file.close();
  reading = false;
}

size_t Tarball::position() {
  if (reading || writing) {
    return file.position();
  } else {
    return -1;
  }
}

//###############################################################################
//  OmniESPUpdater
//###############################################################################

//...............................................................................
//  constructor
//...............................................................................

OmniESPUpdater::OmniESPUpdater(LOGGING &logging) : logging(logging){};

//...............................................................................
//  error message handling
//...............................................................................

void OmniESPUpdater::setErrorMsg(String msg) {
  logging.error(msg);
  errorMsg = msg;
}

String OmniESPUpdater::getLastError() { return errorMsg; }

//...............................................................................
//  untar
//...............................................................................

void OmniESPUpdater::chksum(const char b[END], char *chk) {
  unsigned sum = 0, i;
  for (i = 0; i < END; i++)
    sum += (i >= CHK && i < CHK + 8) ? ' ' : b[i];
  snprintf(chk, 9, "0%5o", sum);
}

bool OmniESPUpdater::extract(Tarball &tarball, char *fname, int l,
                             char b[END]) {
  static char chk[9] = {0};
  int r = 0;
  FILE *f = NULL;
  String filename = fname;
  logging.info("extracting " + filename);

  // unlink(fname);
  switch (b[TYPE]) {
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
    setErrorMsg(filename + ": unsupported filetype");
    return false;
  }
  chksum(b, chk);
  Ds(b + CHK, chk);
  if (strncmp(b + CHK, chk, 6)) {
    setErrorMsg(filename + ": wrong checksum");
    return false;
  }
  char x[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int br = 0, bw = 0;
  for (; l > 0; l -= TARBLOCKSIZE) { // block is always TARBLOCKSIZE
    Di("reading from position:", tarball.position());
    int k = tarball.read((uint8_t *)b, TARBLOCKSIZE);
    br += k;
    Di("bytes read from tarBall:", k);
    strncpy(x, b, 10);
    Ds("begin of block:", x);
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
    bw += MIN(l, TARBLOCKSIZE);
    // if() fwrite(b, MIN(l, TARBLOCKSIZE), 1, f);
  }
  Di("bytes read:", br);
  Di("bytes written:", bw);
  // if(f) fclose(f);
  return true;
};

bool OmniESPUpdater::untar(Tarball &tarball) {
  int l;
  char b[END], fname[101] = {0};

  char x[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  logging.info("extracting files from tarball");
  int k;
  while (k = tarball.read((uint8_t *)b, END)) {
    Di("main loop, bytes read from tarBall:", k);
    Di("now at position:", tarball.position());
    strncpy(x, b, 10);
    Ds("begin of block:", x);
    if (*b == '\0')
      break;
    memcpy(fname, b, 100);
    Ds("filename is:", fname);
    l = strtol(b + SIZE, 0, 8);
    Di("size is:", l) if (!extract(tarball, fname, l, b)) return false;
    Di("now at position:", tarball.position());
  }
  return true;
}

//...............................................................................
//  actual update procedure
//...............................................................................

bool OmniESPUpdater::doUpdate(const char *deviceName) {

  Tarball tarball(deviceName);
  if (!tarball.exists()) {
    setErrorMsg("tarball " + tarball.getFilename() + " not found");
    return false;
  }
  logging.info("starting update");
  if (!tarball.beginRead()) {
    setErrorMsg(tarball.getLastError());
    return false;
  }
  logging.info("using tarball " + tarball.getFilename());
  untar(tarball);
  tarball.endRead();
  logging.info("update finished");
}
