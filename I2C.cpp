#include "I2C.h"

//###############################################################################
//  I2C 
//###############################################################################
I2C::I2C(){
}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------
void I2C::start(){
  Wire.begin(I2C_SDA, I2C_SCL);   //begin(sda, scl)
  scanBus();
  lcd.init();
}

//...............................................................................
//  scan I2C-Bus for devices
//...............................................................................
void I2C::scanBus(){
  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C-Bus......");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);

      if (address == 0x77) {Serial.println(" BMP180");}
      else if (address == 0x3c) {Serial.println(" SSD1306");}
      else if (address == 0x40) {Serial.println(" SI7021");}
      else if (address >= 0x70 & address <= 0x77) {Serial.println(" PCA9544");}
      else if (address >= 0x60 & address <= 0x67) {Serial.println(" MCP4725");}
      else if (address >= 0x38) {Serial.println(" ADE7953");}
      else {Serial.println(" unknown");};
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done");  
  Serial.println("............................................");

}

//###############################################################################
//  LCD Display 
//###############################################################################
LCD::LCD():
    ssd1306(SSD1306_I2C_adr, I2C_SDA, I2C_SCL){
}

//-------------------------------------------------------------------------------
//  FFS public
//-------------------------------------------------------------------------------
//...............................................................................
//  init display
//...............................................................................
void LCD::init(){
  ssd1306.init();
  ssd1306.flipScreenVertically();
  ssd1306.setContrast(255);
}

//...............................................................................
//  printline
//...............................................................................
void LCD::println(String txt, const char *fontData, int yPos){
  ssd1306.setTextAlignment(TEXT_ALIGN_LEFT);
  ssd1306.setFont(fontData);
  ssd1306.drawString(0, yPos, txt);
  ssd1306.display();
}
//...............................................................................
//  clear display
//...............................................................................
void LCD::clear(){
  ssd1306.clear();
}



