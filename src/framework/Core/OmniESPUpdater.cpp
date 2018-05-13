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
  //Ds("tarball removing...", filename.c_str());
  if (!SPIFFS.exists(filename))
    return true;
  //D("tarball do actual remove on SPIFFS");
  if (SPIFFS.remove(filename)) {
    return true;
  } else {
    //D("error removing tarball");
    setErrorMsg("could not delete tarball " + filename);
    return false;
  }
}

bool Tarball::beginWrite() {
  if (writing)
    endWrite();
  if (reading)
    endRead();
  //D("tarball begin write");
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
    D("short write");
    setErrorMsg("short write");
    return false;
  }
}

bool Tarball::endWrite() {
  //D("tarball end write");
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
  String filename = fname;
  File file;

  chksum(b, chk);
  //Ds(b + CHK, chk);
  if (strncmp(b + CHK, chk, 6)) {
    setErrorMsg("wrong checksum for " + filename);
    return false;
  }

  bool extract = skipConfigFiles ? strcmp(fname, DEVICECONFIG) : true;

  switch (b[TYPE]) {
  case REG:
    // regular file
    if (extract) {
      logging.info("extracting file " + filename);
      // open file for writing
      file = SPIFFS.open(filename, "w");
      if (!file) {
        setErrorMsg("could not create file " + filename);
        return false;
      }
    } else
      logging.info("skipping file " + filename);
    // write file
    for (; l > 0; l -= TARBLOCKSIZE) { // block is always TARBLOCKSIZE
      tarball.read((uint8_t *)b, TARBLOCKSIZE);
      if (extract)
        file.write((uint8_t *)b, MIN(l, TARBLOCKSIZE));
    }
    if (extract) {
      // close file
      file.flush();
      file.close();
    }
    break;
  case DIRECTORY:
    // nothing to do, FFS does not support directories
    logging.info("ignoring directory " + filename);
    break;
  default:
    setErrorMsg("unsupported filetype for " + filename);
    return false;
  }
  return true;
};

bool OmniESPUpdater::untar(Tarball &tarball) {
  int l;
  char b[END], fname[101] = {0};

  logging.info("extracting files from tarball");
  while (tarball.read((uint8_t *)b, END)) {
    if (*b == '\0')
      break;
    memcpy(fname, b, 100);
    l = strtol(b + SIZE, 0, 8);
    if (!extract(tarball, fname, l, b))
      return false;
  }
  logging.info("extraction finished");
  return true;
}

//...............................................................................
//  actual update procedure
//...............................................................................

String OmniESPUpdater::getUpdateErrorString(uint8_t error) {
  if (error == UPDATE_ERROR_OK) {
    return ("No Error");
  } else if (error == UPDATE_ERROR_WRITE) {
    return ("Flash Write Failed");
  } else if (error == UPDATE_ERROR_ERASE) {
    return ("Flash Erase Failed");
  } else if (error == UPDATE_ERROR_READ) {
    return ("Flash Read Failed");
  } else if (error == UPDATE_ERROR_SPACE) {
    return ("Not Enough Space");
  } else if (error == UPDATE_ERROR_SIZE) {
    return ("Bad Size Given");
  } else if (error == UPDATE_ERROR_STREAM) {
    return ("Stream Read Timeout");
  } else if (error == UPDATE_ERROR_MD5) {
    return ("MD5 Check Failed");
  } else if (error == UPDATE_ERROR_FLASH_CONFIG) {
    return ("Flash config wrong");
  } else if (error == UPDATE_ERROR_NEW_FLASH_CONFIG) {
    return ("new Flash config wrong");
  } else if (error == UPDATE_ERROR_MAGIC_BYTE) {
    return ("Magic byte is wrong, not 0xE9");
    //  } else if (error == UPDATE_ERROR_BOOTSTRAP) {
    //    return("Invalid bootstrapping state, reset ESP8266 before updating");
  } else {
    return ("Unknown error");
  }
}

bool OmniESPUpdater::doUpdate(const char *deviceName, bool setDeviceDefaults) {

  bool result;

  // untarring files
  Tarball tarball(deviceName);
  if (!tarball.exists()) {
    setErrorMsg("tarball " + tarball.getFilename() + " not found");
    return false;
  }
  logging.info("starting update");
  if(setDeviceDefaults) {
    logging.info("will set device config to defaults");
  }
  if (!tarball.beginRead()) {
    setErrorMsg(tarball.getLastError());
    return false;
  }
  logging.info("using tarball " + tarball.getFilename());
  skipConfigFiles = !setDeviceDefaults;
  result = untar(tarball);
  tarball.endRead();
  if (!result)
    return false;

  // firmware update
  File firmware = SPIFFS.open("firmware/firmware.bin", "r");
  if (!firmware) {
    setErrorMsg("firmware is missing");
    return false;
  }
  size_t l = firmware.size();
  result = Update.begin(l, U_FLASH);
  if (!result) {
    setErrorMsg(getUpdateErrorString(Update.getError()));
  } else {
    logging.info("writing firmware"); /*
     int r;
     uint8_t data[4096];
     while(l> 0) {
       r= tarball.read(data, 4096);
       l-= r;
       Update.write(data, r);
     } */
    Update.writeStream(firmware);
    Update.end();
    result = (Update.getError() == UPDATE_ERROR_OK);
    if (!result)
      setErrorMsg(getUpdateErrorString(Update.getError()));
  }
  firmware.close();
  if(result)
    logging.info("update prepared");
  return result;
}
