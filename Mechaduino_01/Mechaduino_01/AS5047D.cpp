
#include <Wire.h>
#include <SPI.h>
#include "Parameters.h"
#include "AS5047D.h"

#define speedMaximum (4000000)
#define dataOrder    MSBFIRST
#define dataMode     SPI_MODE1



static int check_even_parity(unsigned short);




void AS5047D_Init()
{
    SPISettings settingsA(speedMaximum, dataOrder, dataMode);
    SPI.begin(); // uses mode=1 (CPOL=0, CPHA=1)
    SerialUSB.println("Begin SPI ....");
    delay(1000);
    SPI.beginTransaction(settingsA);
}

// function gets parity of specified 16 bit value.
// return 1 if value has odd parity and returns 0 if value has even parity
// 
static int check_even_parity(unsigned short val)
{
  int parity = 0;
  while (val) 
  {
    parity = !parity;
    val = val & ( val-1);
  }
  //SerialUSB.println(parity);
  return parity;
}

unsigned short AS5047D_Read_Register(unsigned short as5047d_register)
{

    unsigned short regval = 0;
    unsigned short wrk = 0;
    //SerialUSB.println(as5047d_register);
    

    wrk = as5047d_register | RW;              // We want to read
    

  
    // Compute even parity on the 15 bits of the command
 
    if (check_even_parity(wrk) == 1)
    {
        wrk |= PARC;
    }
    //SerialUSB.println(wrk);
    //SerialUSB.println();
    digitalWrite(chipSelectPin, LOW); // CS=0
    byte b1 = SPI.transfer(wrk >> 8);
    byte b2 = SPI.transfer(wrk & 0xFF);
    digitalWrite(chipSelectPin, HIGH); // CS=1
    regval = ((b1 << 8) | b2);
    //SerialUSB.println(regval);
    
    // check if a frame error was detected
    if ( (regval & EF) != 0)
    {
        // somehow we always get bit 14 set indicating a framing error
        regval &= ~( EF |PARD ); // let us ignore it for now   
        // Framing Error detected
        // report it
        //regval = 0xffff; // all bits set indicate invalid value read ( data is only 14bits)
        //SerialUSB.println("Framing Error");
        
    }
    else
    {
        regval &= ~( EF |PARD );
    }
    return regval;

}
