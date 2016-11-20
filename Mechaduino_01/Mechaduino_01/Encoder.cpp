

#include <SPI.h>
#include <Wire.h>
#include "AS5047D.h"
#include "Encoder.h"

#include "Parameters.h"
#include "Utils.h"



// Read Encoder using the AS5047D driver

unsigned short readEncoderNew(int comp)
{
    long angleTemp;
    unsigned short wrk;
    if ( comp == 1)
    {
        wrk = AS5047D_Read_Register(AS5047D_ANGLECOM);
    }
    else
    {
        wrk = AS5047D_Read_Register(AS5047D_ANGLEUNC);
    }
    SerialUSB.println(wrk);
    return wrk;
}



int readEncoder()           //////////////////////////////////////////////////////   READENCODER   ////////////////////////////
{
  long angleTemp;
  digitalWrite(chipSelectPin, LOW);

  //angle = SPI.transfer(0xFF);
  byte b1 = SPI.transfer(0xFF);
  byte b2 = SPI.transfer(0xFF);


  angleTemp = (((b1 << 8) | b2) & 0B0011111111111111);
  //  SerialUSB.println((angle & 0B0011111111111111)*0.02197265625);

  digitalWrite(chipSelectPin, HIGH);
  return angleTemp;
}

void readEncoderDiagnosticsNew()
{
    unsigned short wrk1 = 0;
    unsigned char agc = 0;
    
    // Read Diagnostic and Automatic Gain Control Register
    // and decode the diagnostic information
    wrk1 = AS5047D_Read_Register(AS5047D_DIAAGC);
    if ( wrk1 & DIAAGC_MAGL)
    {
        SerialUSB.print("  MAGL Magnetic field strength too low; AGC=0xFF, ");
    }
    if (wrk1 &  DIAAGC_MAGH)
    {
        SerialUSB.print("  MAGH Magnetic field strength too high; AGC=0x00, ");
    }
    if (wrk1 & DIAAGC_COF)
    {
        SerialUSB.print("  COF CORDIC overflow, ");
    }
    if (wrk1 & DIAAGC_LF )
    {
        SerialUSB.println("  LF  Offset compensation ");
    }
    agc = wrk1 & DIAAGC_AGC_VAL;
    SerialUSB.print("");
}


void readEncoderDiagnostics()           //////////////////////////////////////////////////////   READENCODERDIAGNOSTICS   ////////////////////////////
{
  long angleTemp;
  digitalWrite(chipSelectPin, LOW);


    ///////////////////////////////////////////////READ DIAAGC (0x3FFC)
  SerialUSB.print("DIAAGC (0x3FFC)   ");

  SPI.transfer(0xFF);
  SPI.transfer(0xFC);
  digitalWrite(chipSelectPin, HIGH);

  delay(1);
  digitalWrite(chipSelectPin, LOW);

  byte b1 = SPI.transfer(0xC0);
  byte b2 = SPI.transfer(0x00);


  angleTemp = (((b1 << 8) | b2) & 0B1111111111111111);
  SerialUSB.print((angleTemp | 0B1110000000000000000 ), BIN);

  if (angleTemp & (1 << 14)) {
    SerialUSB.print("  Error occurred  ");
  }
  if (angleTemp & (1 << 11)) {
    SerialUSB.print("  MAGH  ");
  }
  if (angleTemp & (1 << 10)) {
    SerialUSB.print("  MAGL  ");
  }
  if (angleTemp & (1 << 9)) {
    SerialUSB.print("  COF  ");
  }
  if (angleTemp & (1 << 8)) {
    SerialUSB.print("  LF  ");
  }
  SerialUSB.println(" ");

  digitalWrite(chipSelectPin, HIGH);


  delay(1);

  digitalWrite(chipSelectPin, LOW);
  ///////////////////////////////////////////////READ ERRFL (0x0001)
  SerialUSB.print("ERRFL (0x0001)   ");

  SPI.transfer(0x40);
  SPI.transfer(0x01);
  digitalWrite(chipSelectPin, HIGH);

  delay(1);
  digitalWrite(chipSelectPin, LOW);

  b1 = SPI.transfer(0xC0);
  b2 = SPI.transfer(0x00);


  angleTemp = (((b1 << 8) | b2) & 0B1111111111111111);
  SerialUSB.print((angleTemp | 0B1110000000000000000 ), BIN);

  if (angleTemp & (1 << 14)) {
    SerialUSB.print("  Error occurred  ");
  }
  if (angleTemp & (1 << 2)) {
    SerialUSB.print("  parity error ");
  }
  if (angleTemp & (1 << 1)) {
    SerialUSB.print("  invalid register  ");
  }
  if (angleTemp & (1 << 0)) {
    SerialUSB.print("  framing error  ");
  }

  SerialUSB.println(" ");

  digitalWrite(chipSelectPin, HIGH);


  delay(1);

}
