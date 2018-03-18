#include "I2C.h"

//===============================================================================
//  I2C
//===============================================================================
I2C::I2C(string name, LOGGING &logging, TopicQueue &topicQueue, int sda, int scl)
        :Module(name, logging, topicQueue),
         sda(sda), scl(scl)
         {}


//-------------------------------------------------------------------------------
//  I2C public
//-------------------------------------------------------------------------------
//...............................................................................
// start
//...............................................................................
void I2C::start() {
  Module::start();
  logging.info("starting I2C");
  scanBus();
  testBRZO();
}

//...............................................................................
// test BRZO-Library
//...............................................................................
void I2C::testBRZO(){
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  digitalWrite(15, HIGH);
  delay(200);
  digitalWrite(15, LOW);


  uint8_t buffer[10];
  buffer[0] = 0xAB;
  buffer[1] = 0xCD;
  buffer[2] = 0xEF;
  buffer[3] = B01000001;

  writeByte(0x20, 0x0A, buffer[3]);
  Serial.println(readByte(0x20, 0x0A), HEX);


  writeBuf(0x20, 0x00, buffer, 2);
  writeByte(0x20, 0x02, buffer[2]);

  Serial.println(readByte(0x20, 0x00), HEX);
  Serial.println(readByte(0x20, 0x01), HEX);
  Serial.println(readByte(0x20, 0x02), HEX);



/*
uint8_t I2C_ADR = 0x20;
int I2C_fClock  = 100;  //800kHz is max
uint8_t buffer[10];

brzo_i2c_setup(sda, scl, 2000);

  brzo_i2c_start_transaction(I2C_ADR, I2C_fClock);
  //write
    buffer[0] = MCP23017_IODIRB;
    buffer[1] = 0xCD;
    //brzo_i2c_write(buffer, 2, false);
    buffer[0] = B00000000;
    brzo_i2c_write(buffer, 1, false);

  //read
    buffer[0] = MCP23017_IODIRB;
    //brzo_i2c_write(buffer, 1, true);
    //brzo_i2c_read(buffer, 1, false);

    //Serial.println(buffer[0], HEX);

    Serial.println(brzo_i2c_end_transaction(), BIN);

    brzo_i2c_start_transaction(I2C_ADR+1, I2C_fClock);
      buffer[0] = B00000000;
      brzo_i2c_write(buffer, 1, false);
    Serial.println(brzo_i2c_end_transaction(), BIN);
*/
}

//...............................................................................
// handle
//...............................................................................
void I2C::handle() {
  Module::handle();
  unsigned long now = millis();

//poll measurement values
  //if (now - tPoll > I2CPOLL){
    //tPoll = now;
    //readBMP180();
    //readSi7021();
  //}
}

//...............................................................................
// getVersion
//...............................................................................
String I2C::getVersion() {
  return  String(I2C_Name) + " v" + String(I2C_Version);
}

//...............................................................................
//  scan I2C-Bus for devices
//...............................................................................
void I2C::scanBus() {
  byte error, address;
  int nDevices;

  logging.info("scanning I2C bus");
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
      char device[30];
      if (address == 0x77) {
        sprintf(device, "0x%02x BMP180", address);
      } else if (address == 0x3c) {
        sprintf(device, "0x%02x SSD1306", address);
      } else if (address >= 0x20 & address <= 0x27) {
        sprintf(device, "0x%02x MCP23017", address);
      } else if (address == 0x40) {
        sprintf(device, "0x%02x SI7021", address);
      } else if (address >= 0x70 & address <= 0x77) {
        sprintf(device, "0x%02x PCA9544", address);
      } else if (address >= 0x60 & address <= 0x67) {
        sprintf(device, "0x%02x MCP4725", address);
      } else if (address >= 0x38) {
        sprintf(device, "0x%02x ADE7953", address);
      } else {
        sprintf(device, "0x%02x unknown", address);
      };
      logging.info(device);
      nDevices++;
    } else if (error == 4) {
      char txt[50];
      sprintf(txt, "unknown error at address 0x%02x", address);
      logging.info(txt);
    }
  }
  if (nDevices == 0)
    logging.info("no I2C devices found");
  else
    logging.info("I2C bus scan done");
}

//...............................................................................
//  I2C read/write
//...............................................................................
uint8_t I2C::writeByte(uint8_t i2cAddr, uint8_t reg, uint8_t val) {
  uint8_t buffer[2] = {reg, val};

  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, I2C_CLOCK);
    brzo_i2c_write(buffer, 1, false);
  return brzo_i2c_end_transaction();
}

uint8_t I2C::writeBuf(uint8_t i2cAddr, uint8_t address, uint8_t val[], int bytesCount) {
  uint8_t buffer[bytesCount+1];
  buffer[0] = address;
  for (int i = 0; i < bytesCount; i++) {
    buffer[i+1] = val[i];
  }
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, I2C_CLOCK);
  brzo_i2c_write(buffer, bytesCount+1, false);
  return brzo_i2c_end_transaction();
}

uint8_t I2C::readByte(uint8_t i2cAddr, uint8_t address) {
  uint8_t buffer[1] {address};
  brzo_i2c_setup(sda, scl, 2000);
  brzo_i2c_start_transaction(i2cAddr, I2C_CLOCK);
  brzo_i2c_write(buffer, 1, true);
  brzo_i2c_read(buffer, 1, false);
  return buffer[0];
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
