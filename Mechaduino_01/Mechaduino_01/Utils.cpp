//Contains the definitions of the functions used by the firmware.


#include <SPI.h>
#include <Wire.h>


#include "Parameters.h"
#include "Controller.h"
#include "Utils.h"
#include "State.h"
#include "analogFastWrite.h"
#include "Calibration.h"
#include "Encoder.h"
#include "AS5047D.h"











// Display the various parameters

void parameterQuery() {
  SerialUSB.println(' ');
  SerialUSB.println("----Current Parameters-----");
  SerialUSB.println(' ');
  SerialUSB.println(' ');

  SerialUSB.print("volatile float Ts = ");
  SerialUSB.print(Ts, DEC);
  SerialUSB.println(";");
  SerialUSB.println(' ');

  SerialUSB.print("volatile float pKp = ");
  SerialUSB.print(pKp);
  SerialUSB.println(";");

  SerialUSB.print("volatile float pKi = ");
  SerialUSB.print(pKi);
  SerialUSB.println(";");

  SerialUSB.print("volatile float pKd = ");
  SerialUSB.print(pKd);
  SerialUSB.println(";");

  SerialUSB.println(' ');

  SerialUSB.print("volatile float vKp = ");
  SerialUSB.print(vKp);
  SerialUSB.println(";");

  SerialUSB.print("volatile float vKi = ");
  SerialUSB.print(vKi / Ts);
  SerialUSB.println(" * Ts;");

  SerialUSB.print("volatile float vKd = ");
  SerialUSB.print(vKd * Ts);
  SerialUSB.println(" / Ts;");

  SerialUSB.println(' ');

  SerialUSB.println("const PROGMEM float lookup[] = {");
  for (int i = 0; i < 16384; i++) {
    SerialUSB.print(lookup_angle(i));
    SerialUSB.print(", ");
  }
  SerialUSB.println("");
  SerialUSB.println("};");



}


float lookup_angle(int n)
{
  float a_out;
  a_out = pgm_read_float_near(lookup + n);
  return a_out;
}






void print_angle()                ///////////////////////////////////       PRINT_ANGLE   /////////////////////////////////
{
  // We average the reading of the encoder over 10 samples
  // we may want to have rather a function encoderAverage with a input parameter indicating the number of samples the average 
  // should be calculated over 
  raw_encoder = 0;
  delay(100);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder += readEncoder();
  delay(10);
  raw_encoder = raw_encoder / 10;

  anglefloat = raw_encoder * 0.02197265625;  // 360 : 16384   14 bits
  SerialUSB.print(stepNumber, DEC);
  SerialUSB.print(" , ");
  SerialUSB.print(stepNumber * aps, DEC);
  SerialUSB.print(" , ");
  SerialUSB.print(raw_encoder, DEC);
  SerialUSB.print(" , ");
  SerialUSB.println(anglefloat, DEC);
}


void receiveEvent(int howMany)
{
  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    SerialUSB.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  SerialUSB.println(x);         // print the integer
  r = 0.1 * ((float)x);   // r is a global find a better name for it
}




int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}



float lookup_force(int m)        /////////////////////////////////////////////////  LOOKUP_force   /////////////////////////////
{
  float b_out;
  //
  //  m = (0.01*(((m % 62832) + 62832) % 62832))+0.5;  //+0.5 for rounding
  //
  //  //SerialUSB.println(m);
  //
  //  if (m > 314) {
  //    m = m - 314;
  //    b_out = -pgm_read_float_near(force_lookup + m);
  //
  //  }
  //  else
  //  {
  b_out = pgm_read_float_near(force_lookup + m);
  //  }

  return b_out;
}





float lookup_sine(int m)        /////////////////////////////////////////////////  LOOKUP_SINE   /////////////////////////////
{
  float b_out;

  m = (0.01 * (((m % 62832) + 62832) % 62832)) + 0.5; //+0.5 for rounding

  //SerialUSB.println(m);

  if (m > 314) {
    m = m - 314;
    b_out = -pgm_read_float_near(sine_lookup + m);

  }
  else
  {
    b_out = pgm_read_float_near(sine_lookup + m);
  }

  return b_out;
}



// This function requires the understanding of the interal features of the SOC 
// In this particular case we manipulate the register used in controlling the 
// Timers


void setupTCInterrupts() {



  // Enable GCLK for TC4 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.CC[0].reg = 0x3E72; //0x4AF0;
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC5->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC5->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0


  NVIC_SetPriority(TC5_IRQn, 1);

  
  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);

    
  // Enable TC
  //  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  //  WAIT_TC16_REGS_SYNC(TC5)




}

void enableTCInterrupts() {

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}

void disableTCInterrupts() {


  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}


 
 
void parameterEditmain() {

    SerialUSB.println();
    SerialUSB.println("Edit parameters:");
    SerialUSB.println();
    SerialUSB.println("p ----- proportional loop");
    SerialUSB.println("v ----- velocity loop");
    SerialUSB.println("o ----- other");
    SerialUSB.println("q ----- quit");
    SerialUSB.println();

    while (SerialUSB.available() == 0)  {}
    char inChar2 = (char)SerialUSB.read();

    switch (inChar2) {
      case 'p':
      {
         parameterEditp();
      }
        break;

      case 'v':
      {    
       parameterEditv();     
      }      
        break;

      case 'o':
      {
        parameterEdito();
      }
        break;
      default:
      {}
        break;
    
   

          }
}

void parameterEditp(){


  SerialUSB.println("Edit position loop gains:");
        SerialUSB.println();
        SerialUSB.print("p ----- pKp = ");
        SerialUSB.println(pKp,DEC);
        SerialUSB.print("i ----- pKi = ");
        SerialUSB.println(pKi,DEC);
        SerialUSB.print("d ----- pKd = ");
        SerialUSB.println(pKd,DEC);
        SerialUSB.println("q ----- quit");
        SerialUSB.println();
        
        while (SerialUSB.available() == 0)  {}
        char inChar3 = (char)SerialUSB.read();
        
        switch (inChar3) {
            case 'p':
              {
              SerialUSB.println("pKp = ?");
              while (SerialUSB.available() == 0)  {}
              pKp = SerialUSB.parseFloat();
              SerialUSB.print("new pKp = ");
              SerialUSB.println(pKp,DEC);
              }
              break;
            case 'i':
              {
              SerialUSB.println("pKi = ?");
              while (SerialUSB.available() == 0)  {}
              pKi = SerialUSB.parseFloat();
              SerialUSB.print("new pKi = ");
              SerialUSB.println(pKi,DEC);
              }
              break;
            case 'd':  
              {
              SerialUSB.println("pKd = ?");
              while (SerialUSB.available() == 0)  {}
              pKd = SerialUSB.parseFloat();
              SerialUSB.print("new pKd = ");
              SerialUSB.println(pKd,DEC);
              }
              break;
            default:
            {}
              break;
        }
}

void parameterEditv(){
  SerialUSB.println("Edit velocity loop gains:");
  SerialUSB.println();
  SerialUSB.print("p ----- vKp = ");
  SerialUSB.println(vKp,DEC);
  SerialUSB.print("i ----- vKi = ");
  SerialUSB.println(vKi,DEC);
  SerialUSB.print("d ----- vKd = ");
  SerialUSB.println(vKd,DEC);
  SerialUSB.println("q ----- quit");
  SerialUSB.println();
  
  while (SerialUSB.available() == 0)  {}
  char inChar4 = (char)SerialUSB.read();
  
  switch (inChar4) {
      case 'p':
        {
        SerialUSB.println("vKp = ?");
        while (SerialUSB.available() == 0)  {}
        vKp = SerialUSB.parseFloat();
        SerialUSB.print("new vKp = ");
        SerialUSB.println(vKp,DEC);
        }
        break;
      case 'i':
        {
        SerialUSB.println("vKi = ?");
        while (SerialUSB.available() == 0)  {}
        vKi = SerialUSB.parseFloat();
        SerialUSB.print("new vKi = ");
        SerialUSB.println(vKi,DEC);
        }
        break;
      case 'd':  
        {
        SerialUSB.println("vKd = ?");
        while (SerialUSB.available() == 0)  {}
        vKd = SerialUSB.parseFloat();
        SerialUSB.print("new vKd = ");
        SerialUSB.println(vKd,DEC);
        }
        break;
      default:
      {}
        break;             
  }
}

void parameterEdito(){


  SerialUSB.println("Edit other parameters:");
        SerialUSB.println();
        SerialUSB.print("p ----- PA = ");
        SerialUSB.println(PA,DEC);
        SerialUSB.println();

        
        while (SerialUSB.available() == 0)  {}
        char inChar3 = (char)SerialUSB.read();
        
        switch (inChar3) {
            case 'p':
              {
              SerialUSB.println("PA = ?");
              while (SerialUSB.available() == 0)  {}
              PA = SerialUSB.parseFloat();
              SerialUSB.print("new PA = ");
              SerialUSB.println(PA,DEC);
              }
       
              break;
            default:
            {}
              break;
        }
}


