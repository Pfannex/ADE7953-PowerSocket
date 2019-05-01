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
  // Ds("tarball removing...", filename.c_str());
  if (!SPIFFS.exists(filename))
    return true;
  // D("tarball do actual remove on SPIFFS");
  if (SPIFFS.remove(filename)) {
    return true;
  } else {
    // D("error removing tarball");
    setErrorMsg("could not delete tarball " + filename);
    return false;
  }
}

bool Tarball::beginWrite() {
  if (writing)
    endWrite();
  if (reading)
    endRead();
  // D("tarball begin write");
  file = SPIFFS.open(filename, "w");
  if (!file) {
    setErrorMsg("could not create tarball " + filename);
  } else {
    writing = true;
  }
  return writing;
}

bool Tarball::write(uint8_t *data, size_t len) {
  // Di("tarball write bytes:", len);
  if (file.write(data, len) == len)
    return true;
  else {
    // D("short write");
    setErrorMsg("short write");
    return false;
  }
}

bool Tarball::endWrite() {
  // D("tarball end write");
  file.flush();
  file.close();
  writing = false;
  return true;
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
  return true;
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
  /*
  The checksum is calculated by taking the sum of the unsigned byte values
  of the header record with the eight checksum bytes taken to be ascii
  spaces (decimal value 32). It is stored as a six digit octal number with
  leading zeroes followed by a NUL and then a space. Various implementations
  do not adhere to this format. For better compatibility, ignore leading and
  trailing whitespace, and take the first six digits.
  */
  unsigned sum = 0, i;
  for (i = 0; i < END; i++)
    sum += (i >= CHK && i < CHK + 8) ? ' ' : b[i];
  snprintf(chk, 9, "%06o  ", sum);
}

bool OmniESPUpdater::extract(Tarball &tarball, char *fname, int l,
                             char b[END]) {
  String filename = fname;
  // prepending / is absolutely necessary, else the files go to nirvana
  if (!filename.startsWith("/"))
    filename = "/" + filename;

  static char chk[9] = {0};
  chksum(b, chk);
  if (strncmp(b + CHK, chk, 6)) {
    setErrorMsg("wrong checksum for " + filename);
    //Ds("actual: ", chk);
    //Ds("tar   : ", b + CHK);
    return false;
  }

  bool extract = skipConfigFiles ? strcmp(fname, DEVICECONFIG) : true;
  bool flash = !strcmp(fname, FIRMWAREBIN);
  // (note: ensure that checking for flash preceded checking for extract)

  File file;
  switch (b[TYPE]) {
  case REG:
    //
    // regular file
    //
    // 1. initialization
    if (flash) {
      // prepare flashing
      logging.info("flashing file " + filename);
      logging.info("firmware size is " + String(l, DEC) + " bytes");
      bool flash = Update.begin(l, U_FLASH);
      if (!flash) {
        setErrorMsg(getUpdateErrorString(Update.getError()));
        return false;
      };
    } else if (extract) {
      // prepare extraction
      logging.info("extracting file " + filename);
      // open file for writing
      file = SPIFFS.open(filename, "w");
      if (!file) {
        setErrorMsg("could not create file " + filename);
        return false;
      }
    } else {
      // ignore the file
      logging.info("skipping file " + filename);
    }
    // 2. processing
    for (; l > 0; l -= TARBLOCKSIZE) { // block is always TARBLOCKSIZE
      tarball.read((uint8_t *)b, TARBLOCKSIZE);
      if (flash) {
        Update.write((uint8_t *)b, MIN(l, TARBLOCKSIZE));
      } else if (extract) {
        // Di("extracting", MIN(l, TARBLOCKSIZE));
        file.write((uint8_t *)b, MIN(l, TARBLOCKSIZE));
      }
    }
    // 3. finalization
    if (flash) {
      // end update
      Update.end();
      if(Update.getError() != UPDATE_ERROR_OK) {
        setErrorMsg(getUpdateErrorString(Update.getError()));
        return false;
      }
    } else if (extract) {
      // close file
      file.close();
    }
    break;
  case DIRECTORY:
    //
    // directory
    //
    // nothing to do, FFS does not support directories
    logging.info("ignoring directory " + filename);
    break;
  default:
    //
    // other, unsupported, should not occur
    //
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

void OmniESPUpdater::listSpiffs() {

  logging.debug("FFS listing:");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    logging.debug("  " + dir.fileName() + ", " + String(dir.fileSize(), DEC));
  }
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

  Tarball tarball(deviceName);
  if (extractFiles(tarball, setDeviceDefaults)) {
    listSpiffs();
    tarball.remove(); // remove tarball, free space
    return true;
  } else
    return false;
}

bool OmniESPUpdater::extractFiles(Tarball tarball, bool setDeviceDefaults) {

  // untarring files
  if (!tarball.exists()) {
    setErrorMsg("tarball " + tarball.getFilename() + " not found");
    return false;
  }
  logging.info("starting update");
  if (setDeviceDefaults) {
    logging.info("will set device config to defaults");
  }
  if (!tarball.beginRead()) {
    setErrorMsg(tarball.getLastError());
    return false;
  }
  logging.info("using tarball " + tarball.getFilename());
  skipConfigFiles = !setDeviceDefaults;
  bool result = untar(tarball);
  tarball.endRead();
  if (!result)
    return false;
}

bool OmniESPUpdater::flash(String filename) {

  // firmware update
  // note: ensure that filename has a leading slash, like in "/firmware/firmware.bin"
  File firmware = SPIFFS.open(filename, "r");
  if (!firmware) {
    setErrorMsg("firmware is missing");
    return false;
  }
  size_t l = firmware.size();
  logging.info("firmware size is " + String(l, DEC) + " bytes");

  bool result = Update.begin(l, U_FLASH);
  if (!result) {
    setErrorMsg(getUpdateErrorString(Update.getError()));
  } else {
    logging.info("writing firmware");
    /*
     int r;
     uint8_t data[512];
     while(firmware.available()) {
       r= firmware.read(data, 512);
       Update.write(data, r);
       Di("firmware bytes written:", r);
     }*/
    Update.writeStream(firmware);
    Update.end();
    result = (Update.getError() == UPDATE_ERROR_OK);
    if (!result)
      setErrorMsg(getUpdateErrorString(Update.getError()));
  }
  firmware.close();
  if (result)
    logging.info("firmware written");

  return result;
}
