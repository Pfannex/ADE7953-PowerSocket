#include "I2C.h"

//===============================================================================
//  I2C
//===============================================================================
I2C::I2C(){
}

//-------------------------------------------------------------------------------
//  I2C public
//-------------------------------------------------------------------------------

//...............................................................................
//  I2C-Bus begin
//...............................................................................
void I2C::begin(){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
}
void I2C::begin(uint8_t i2cAddr){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, I2C_CLOCK);
}
void I2C::begin(uint8_t i2cAddr, int clockSpeed){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
}

//...............................................................................
//  I2C-Bus end
//...............................................................................
uint8_t I2C::end(){
  return brzo_i2c_end_transaction();
}

//...............................................................................
//  set I2C-Bus GPIO-pins
//...............................................................................
void I2C::setWire(int sda, int scl){
  sda = sda;
  scl = scl;
}
//...............................................................................
//  set I2C-Bus address & speed
//...............................................................................
void I2C::setDevice(uint8_t i2cAddr, int clockSpeed){
  i2cAddr = i2cAddr;
  clockSpeed = clockSpeed;
}

//...............................................................................
//  scan I2C-Bus for devices
//...............................................................................
String I2C::scanBus() {

  byte error, address;
  int nDevices;
  String device;
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
      if (address == 0x77) {
        device = "BMP180";
      } else if (address == 0x3c) {
        device = "SSD1306";
      } else if (address >= 0x20 & address <= 0x27) {
        device = "MCP23017";
      } else if (address == 0x40) {
        device = "SI7021";
      } else if (address >= 0x70 & address <= 0x77) {
        device = "PCA9544";
      } else if (address >= 0x60 & address <= 0x67) {
        device = "MCP4725";
      } else if (address >= 0x38) {
        device = "ADE7953";
      } else {
        device = "unknown";
      };

      if (String(device) != ""){
        char adr[30];
        sprintf(adr, "\"0x%02X\"", address);
        device = String(adr) + ":\"" + device + "\",";
        json += device;

        #if (DEBUG_I2C == 1)
          Serial.println(String(adr) + ":" + device);
        #endif
      }
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
//write byte
uint8_t I2C::write(uint8_t val){
  uint8_t buffer[1];
  buffer[0] = val;
  brzo_i2c_write(buffer, 1, false);
}
uint8_t I2C::write(uint8_t val, bool repeated_start){
  uint8_t buffer[1];
  buffer[0] = val;
  brzo_i2c_write(buffer, 1, repeated_start);
}
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

//...............................................................................
//  I2C read/write
//...............................................................................
//read byte
uint8_t* I2C::read(uint8_t val[]){
  brzo_i2c_read(val, 1, false);
  return val;
}
uint8_t* I2C::read(uint8_t val[], bool repeated_start){
  brzo_i2c_read(val, 1, repeated_start);
  return val;
}
//buffer[]
uint8_t* I2C::read(uint8_t i2cAddr, int clockSpeed,
                   uint32_t reg, uint8_t regSize,
                   uint8_t buf[], uint8_t bufSize){
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, clockSpeed);
  uint8_t buffer[regSize];
  for (uint8_t i = 0; i < regSize; i++) {
    buffer[i] = (reg >> (regSize-1-i)*8);     //write MSB first
  }
  //uint8_t x[bufSize];
  brzo_i2c_write(buffer, regSize, true);
  brzo_i2c_read(buf, bufSize, false);
  brzo_i2c_end_transaction();
  return buf;
}
//register 8-Bit / val 8-Bit
uint8_t I2C::read8_8(uint8_t reg){
  uint8_t buf[1];
  uint8_t* buffer = read(i2cAddr, clockSpeed, reg, 1, buf, 1);
  return buffer[0];
}
//register 8-Bit / val 16-Bit
uint16_t I2C::read8_16(uint8_t reg){
  uint8_t buf[2];
  uint8_t* buffer = read(i2cAddr, clockSpeed, reg, 1, buf, 2);
  return (buffer[0] << 8) + (buffer[1]);
}
//register 16-Bit / val 8-Bit
uint8_t I2C::read16_8(uint16_t reg){
  uint8_t buf[1];
  uint8_t* buffer = read(i2cAddr, clockSpeed, reg, 2, buf, 1);
  return buffer[0];
}
//register 16-Bit / val 16-Bit
uint16_t I2C::read16_16(uint16_t reg){
  uint8_t buf[2];
  uint8_t* buffer = read(i2cAddr, clockSpeed, reg, 2, buf, 2);
  return (buffer[0] << 8) + buffer[1];
}
//register 16-Bit / val 32-Bit
uint32_t I2C::read16_32(uint16_t reg){
  uint8_t buf[4];
  uint8_t* buffer = read(i2cAddr, clockSpeed, reg, 2, buf, 4);
  return (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
}

//-------------------------------------------------------------------------------
//  GPIO private
//-------------------------------------------------------------------------------

/*
//...............................................................................
// read BMP180
//...............................................................................
void I2C::readBMP180() {
   Adafruit_BMP085 bmp;
   String eventPrefix= "~/event/device/" + String(name) + "/BMP180/";

   bmp.begin();
   String value = "temperature " + String(bmp.readTemperature());
   logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "pressure " + String(bmp.readPressure()/100);
   logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}
//...............................................................................
// read SI7021
//...............................................................................
void I2C::readSi7021() {
   Adafruit_Si7021 si;
   String eventPrefix= "~/event/device/" + String(name) + "/SI7021/";

   si.begin();
   String value = "temperature " + String(si.readTemperature());
   logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
   value = "humidity " + String(si.readHumidity());
   logging.debug(value);
   topicQueue.put(eventPrefix + "/" + value);
}
*/


//...............................................................................
// test I2C-Library
//...............................................................................

void I2C::testI2C(){
  pinMode(15, OUTPUT);
  digitalWrite(15, HIGH);
  delay(200);
  digitalWrite(15, LOW);


// RAW write / read---------------------------------------
  begin(0x20, 400);
    write(0x01, true);
    write(0x33);
  end();

  begin(0x20, 400);
    write(0x01, true);
    uint8_t buf[1];
    uint8_t* buffer = read(buf);
    Serial.println(buf[0], HEX);
  end();

// write / read bytes by size-------------------------------
  setDevice(0x20, 400);
  write8_8(0x00, 0xAB);
  Serial.println(read8_8(0x00), HEX);
  write8_16(0x00, 0xCDEF);
  Serial.println(read8_16(0x00), HEX);

  setDevice(0x38, 400);
  write16_8(0x0000, 0xFA);
  Serial.println(read16_8(0x00), HEX);
  write16_16(0x100, 0xABCD);
  Serial.println(read16_16(0x100), HEX);
  write16_32(0x300, 0x00ABCDEF);
  Serial.println(read16_32(0x300), HEX);

// write / read by buffer[]----------------------------------




/*
  write(0x20, 400, 0xABCD, 2, 0x1A2B3C4D, 4);
  write(0x20, 400, 0xAB, 1, 0x1A2B, 2);
  delay(5);

  setBus(0x20, 100);
  write(0xAB, 0xEF);
  setBus(0x20, 200);
  write_8_16(0xAB, 0xCDEF);
  setBus(0x20, 400);
  write_16_16(0xABCD, 0xFAAF);
  setBus(0x20, 600);
  write_16_32(0xB00B, 0xB00BA00A);


  setBus(0x20, 400);
  write(0x00, 0xAA);
  write(0x01, 0xBB);
  write(0x02, 0xCC);
  uint8_t arr[3];
  uint8_t* buffer = read(0x20, 400, 0x00, 1, arr, 3);
  Serial.println(buffer[0], HEX);
  Serial.println(buffer[1], HEX);
  Serial.println(buffer[2], HEX);

  Serial.println(read(0x00), HEX);
  Serial.println(read(0x01), HEX);
  Serial.println(read(0x02), HEX);

  Serial.println(read8_16(0x00), HEX);


  setBus(0x38, 200);
  write16_8(0x004, 0xFB);
  Serial.println(read16_8(0x004), HEX);
  write16_16(0x101, 0xFBBF);
  Serial.println(read16_16(0x101), HEX);
  write16_32(0x300, 0x00AABBCC);
  Serial.println(read16_32(0x300), HEX);

*/
}
