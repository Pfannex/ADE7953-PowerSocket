#include "I2C.h"

//===============================================================================
//  I2C
//===============================================================================
I2C::I2C(){
  for(uint8_t _i2cAddr= 0; _i2cAddr< 128; _i2cAddr++)
    addresses[_i2cAddr]= false;
}

//-------------------------------------------------------------------------------
//  I2C public
//-------------------------------------------------------------------------------

//...............................................................................
//  I2C-Bus begin
//...............................................................................
void I2C::start(uint8_t _i2cAddr, int _clockSpeed){
  brzo_i2c_setup(sda, scl, 2000);
  setDevice(i2cAddr, clockSpeed);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
}
void I2C::start(uint8_t _i2cAddr){
  start(_i2cAddr, clockSpeed);
}
void I2C::start(){
  start(i2cAddr, clockSpeed);
}

//...............................................................................
//  I2C-Bus end
//...............................................................................
uint8_t I2C::stop(){
  return brzo_i2c_end_transaction();
}

//...............................................................................
//  set I2C-Bus GPIO-pins
//...............................................................................
void I2C::setWire(int _sda, int _scl){
  sda = _sda;
  scl = _scl;
}
//...............................................................................
//  set I2C-Bus address & speed
//...............................................................................
void I2C::setDevice(uint8_t _i2cAddr, int _clockSpeed){
  i2cAddr = _i2cAddr;
  clockSpeed = _clockSpeed;
}

//...............................................................................
//  scan I2C-Bus for devices
//...............................................................................

// need to run scanBus() first to detect devices
bool I2C::isPresent(uint8_t _i2cAddr) {
  return addresses[_i2cAddr];
}


String I2C::deviceAdd(String& devices, String device) {
  if(devices.length()) devices.concat(",");
  devices.concat(device);
  return devices;
}

String I2C::scanBus() {

  byte error, address;
  int nDevices;
  String devices;
  String json = "{";

  #if (DEBUG_I2C == 1)
    Serial.println("scanning I2C bus");
  #endif
  uint8_t buffer[1] = {0};
  nDevices = 0;

  brzo_i2c_setup(sda, scl, 2000);
  for (address = 1; address < 127; address++) {

    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.

    brzo_i2c_start_transaction(address, I2C_CLOCK);
    brzo_i2c_write(buffer, 1, false);
    error = brzo_i2c_end_transaction();

    addresses[address]= false;
    if (error == 0) {
      /*
      returns 0 if transaction completed successfully or error code encoded as
      follows
    	Bit 0 (1) : Bus not free, i.e. either SDA or SCL is low
    	Bit 1 (2) : Not ACK ("NACK") by slave during write: Either the slave did
                  not respond to the given slave address;
    	            or it did not ACK a byte transferred by the master.
    	Bit 2 (4) : Not ACK ("NACK") by slave during read, i.e. slave did not
                  respond to the given slave address
    	Bit 3 (8) : Clock Stretching by slave exceeded maximum clock stretching time
    	Bit 4 (16): Function called with 0 bytes to be read by the master.
    	            Command not sent to the slave, since this could yield to a
                  bus stall (SDA remains 0)
      */
      addresses[address]= true;
      devices = "";
      if (address == 0x76 | address == 0x77) { 
        deviceAdd(devices,  "BMP280");
        deviceAdd(devices,  "BME280");
      }
      if (address == 0x3c) deviceAdd(devices,  "SSD1306");
      if (address >= 0x20 & address <= 0x27) deviceAdd(devices,  "MCP23017");
      if (address == 0x40) deviceAdd(devices,  "SI7021");
      if (address >= 0x48 & address <= 0x4b) deviceAdd(devices,  "ADS1115");
      if (address >= 0x70 & address <= 0x77) deviceAdd(devices,  "PCA9544");
      if (address >= 0x60 & address <= 0x67) deviceAdd(devices,  "MCP4725");
      if (address == 0x38) deviceAdd(devices,  "ADE7953");
      if (address == 0x38 | address == 0x39) deviceAdd(devices,  "VEML6070");
      if (address == 0x23 | address == 0x5c) deviceAdd(devices,  "BH1750");
      if(!devices.length()) devices= "unknown";

      char adr[30];
      sprintf(adr, "\"0x%02X\"", address);
      devices = String(adr) + ":\"" + devices + "\",";
      json += devices;

      #if (DEBUG_I2C == 1)
        Serial.println(String(adr) + ":" + devices);
      #endif

      nDevices++;
    } else if (error == 4) {
      char txt[50];
      sprintf(txt, "unknown error at address 0x%02x", address);
      #if (DEBUG_I2C == 1)
        Serial.println(txt);
      #endif
    }
  }
  json.remove(json.length()-1);
  json += "}";

  if (nDevices == 0){
    #if (DEBUG_I2C == 1)
      Serial.println("no I2C devices found");
    #endif
    return "no I2C devices found";
  }else{
    #if (DEBUG_I2C == 1)
      Serial.println("I2C bus scan done");
    #endif
    return json;
  }
}

//...............................................................................
//  I2C write
//...............................................................................
//buffer[]
uint8_t I2C::write(uint8_t i2cAddr, int clockSpeed, uint8_t buf[], int bufCount){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
    brzo_i2c_write(buf, bufCount, false);
  return brzo_i2c_end_transaction();
}
//register (8/16Bit) / value (8/16/24/32Bit)
uint8_t I2C::write(uint8_t i2cAddr, int clockSpeed,
                   uint16_t reg, uint8_t regSize,
                   uint32_t val, uint8_t valSize){

  uint8_t buffer[regSize+valSize];
  for (uint8_t i = 0; i < regSize; i++) {
    buffer[i] = (reg >> (regSize-1-i)*8);     //write MSB first
    //Serial.println("buffer["+String(i)+"] = reg >> " + String((regSize-1-i)*8));
  }
  for (uint8_t i = regSize; i < valSize+regSize; i++) {
    buffer[i] = (val >> (valSize-1-i+regSize)*8);   //write MSB first
    //Serial.println("buffer["+String(i)+"] = val >> " + String((valSize-1-i+regSize)*8));
  }
  return write(i2cAddr, clockSpeed, buffer, regSize+valSize);
}
//8 bit register / buffer
uint8_t I2C::write8(uint8_t reg, uint8_t val[], int valSize){
  uint8_t regSize = 1;
  uint8_t buffer[regSize+valSize];
  buffer[0] = reg;
  for (uint8_t i = regSize; i < valSize+regSize; i++) {
    buffer[i] = val[i-regSize];
  }
  return write(i2cAddr, clockSpeed, buffer, regSize+valSize);
}
//16 bit register / buffer
uint8_t I2C::write16(uint16_t reg, uint8_t val[], int valSize){
  uint8_t regSize = 2;
  uint8_t buffer[regSize+valSize];
  buffer[0] = reg >> 8;
  buffer[1] = reg;
  for (uint8_t i = regSize; i < valSize+regSize; i++) {
    buffer[i] = val[i-regSize];
  }
  return write(i2cAddr, clockSpeed, buffer, regSize+valSize);
}
//32 bit register / buffer
uint8_t I2C::write32(uint32_t reg, uint8_t val[], int valSize){
  uint8_t regSize = 4;
  uint8_t buffer[regSize+valSize];
  buffer[0] = reg >> 24;
  buffer[1] = reg >> 16;
  buffer[2] = reg >> 8;
  buffer[3] = reg;
  for (uint8_t i = regSize; i < valSize+regSize; i++) {
    buffer[i] = val[i-regSize];
  }
  return write(i2cAddr, clockSpeed, buffer, regSize+valSize);
}
//register 8 Bit / val 8Bit
uint8_t I2C::write8_8(uint8_t reg, uint8_t val){
  uint8_t buffer[2];
  buffer[0] = reg;
  buffer[1] = val;
  return write(i2cAddr, clockSpeed, buffer, 2);
}
//register 8 Bit / val 16Bit
uint8_t I2C::write8_16(uint8_t reg, uint16_t val){
  uint8_t buffer[3];
  buffer[0] = reg;
  buffer[1] = val >> 8;
  buffer[2] = val;
  return write(i2cAddr, clockSpeed, buffer, 3);
}
//register 16 Bit / val 8Bit
uint8_t I2C::write16_8(uint16_t reg, uint8_t val){
  uint8_t buffer[3];
  buffer[0] = reg >> 8;
  buffer[1] = reg;
  buffer[2] = val;
  return write(i2cAddr, clockSpeed, buffer, 3);
}
//register 16 Bit / val 16Bit
uint8_t I2C::write16_16(uint16_t reg, uint16_t val){
  uint8_t buffer[4];
  buffer[0] = reg >> 8;
  buffer[1] = reg;
  buffer[2] = val >> 8;
  buffer[3] = val;
  return write(i2cAddr, clockSpeed, buffer, 4);
}
//register 16 Bit / val 32Bit
uint8_t I2C::write16_32(uint16_t reg, uint32_t val){
  uint8_t buffer[6];
  buffer[0] = reg >> 8;
  buffer[1] = reg;
  buffer[2] = val >> 24;
  buffer[3] = val >> 16;
  buffer[4] = val >> 8;
  buffer[5] = val;
  return write(i2cAddr, clockSpeed, buffer, 6);
}

//write byte RAW
uint8_t I2C::writeRAW(uint8_t val){
  uint8_t buffer[1];
  buffer[0] = val;
  brzo_i2c_write(buffer, 1, false);
}
uint8_t I2C::writeRAW(uint8_t val, bool repeated_start){
  uint8_t buffer[1];
  buffer[0] = val;
  brzo_i2c_write(buffer, 1, repeated_start);
}

//...............................................................................
//  I2C read/write
//...............................................................................
//buffer[]
void I2C::read(uint8_t i2cAddr, int clockSpeed,
               uint32_t reg, uint8_t regSize,
               uint8_t *val, uint8_t valSize){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
  uint8_t buffer[regSize];
  for (uint8_t i = 0; i < regSize; i++) {
    buffer[i] = (reg >> (regSize-1-i)*8);     //write MSB first
  }
  //uint8_t x[bufSize];
  brzo_i2c_write(buffer, regSize, true);
  brzo_i2c_read(val, valSize, false);
  brzo_i2c_end_transaction();
}
void I2C::read8(uint8_t reg, uint8_t *val, int valCount){
  read(i2cAddr, clockSpeed, reg, 1, val, valCount);
}
void I2C::read16(uint16_t reg, uint8_t *val, int valCount){
  read(i2cAddr, clockSpeed, reg, 2, val, valCount);
}
void I2C::read32(uint32_t reg, uint8_t *val, int valCount){
  read(i2cAddr, clockSpeed, reg, 4, val, valCount);
}
//register 8-Bit / val 8-Bit
uint8_t I2C::read8_8(uint8_t reg){
  uint8_t buf[1];
  read(i2cAddr, clockSpeed, reg, 1, buf, 1);
  return buf[0];
}
//register 8-Bit / val 16-Bit
uint16_t I2C::read8_16(uint8_t reg){
  uint8_t buf[2];
  read(i2cAddr, clockSpeed, reg, 1, buf, 2);
  return (buf[0] << 8) + (buf[1]);
}
//register 16-Bit / val 8-Bit
uint8_t I2C::read16_8(uint16_t reg){
  uint8_t buf[1];
  read(i2cAddr, clockSpeed, reg, 2, buf, 1);
  return buf[0];
}
//register 16-Bit / val 16-Bit
uint16_t I2C::read16_16(uint16_t reg){
  uint8_t buf[2];
  read(i2cAddr, clockSpeed, reg, 2, buf, 2);
  return (buf[0] << 8) + buf[1];
}
//register 16-Bit / val 32-Bit
uint32_t I2C::read16_32(uint16_t reg){
  uint8_t buf[4];
  read(i2cAddr, clockSpeed, reg, 2, buf, 4);
  return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

//read byte RAW
void I2C::readRAW(uint8_t *buf){
  brzo_i2c_read(buf, 1, false);
}
void I2C::readRAW(uint8_t *buf, bool repeated_start){
  brzo_i2c_read(buf, 1, repeated_start);
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------



//...............................................................................
// test I2C-Library
//...............................................................................

void I2C::testI2C(){
  uint8_t buf[5];

  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);

  buf[0] = 0xD0;
  setDevice(0x77, 100);
  start(0x77, 400);
    writeRAW(buf[0], true);
    readRAW(buf);
  stop();
  Serial.println(buf[0], HEX);

  buf[0] = 0xD0;
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(0x77, 100);
    brzo_i2c_write(buf, 1, true);
    brzo_i2c_read(buf, 1, false);
  brzo_i2c_end_transaction();
  Serial.println(buf[0], HEX);

  delay(50);
  digitalWrite(15, LOW);




/*
// RAW write / read---------------------------------------
  start(0x20, 400);
    writeRAW(0x01, true);
    writeRAW(0x33);
  stop();

  start(0x20, 400);
    writeRAW(0x01, true);
    readRAW(buf);
    Serial.println(buf[0], HEX);
  stop();

// write / read bytes by size-------------------------------
  setDevice(0x20, 400);
  write8_8(0x00, 0xAB);
  Serial.println(read8_8(0x00), HEX);
  write8_16(0x00, 0xCDEF);
  Serial.println("--------------");
  uint8_t val[] = {0xBB, 0xCC};
  write8(0x00, val, 2);
  Serial.println(read8_16(0x00), HEX);

  read8(0x00, buf, 1);
  Serial.println(buf[0], HEX);

  setDevice(0x38, 400);
  write16_8(0x0000, 0xFA);
  Serial.println(read16_8(0x00), HEX);
  write16_16(0x100, 0xF00F);
  Serial.println(read16_16(0x100), HEX);
  write16_32(0x300, 0x00ABCDEF);
  Serial.println(read16_32(0x300), HEX);

// write / read by buffer[]----------------------------------
  read16(0x100, buf, 2);
  Serial.println("--------------");
  Serial.println(buf[0], HEX);
  Serial.println(buf[1], HEX);
*/
}
