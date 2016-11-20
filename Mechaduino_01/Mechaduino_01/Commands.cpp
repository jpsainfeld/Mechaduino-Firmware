#include <SPI.h>
#include <Wire.h>
#include "Utils.h"
#include "Parameters.h"
#include "Calibration.h"
#include "Encoder.h"
#include "State.h"




void processCommands() {

  if (SerialUSB.available()) {

    char inChar = (char)SerialUSB.read();

    switch (inChar) {


      case 'p':             //print
        print_angle();
        break;

      case 's':             //step
        oneStep();
        print_angle();
        break;

      case 'd':             //dir
        if (dir == 1) {
          dir = 0;
        }
        else {
          dir = 1;
        }
        break;

      case 'w':
        calibration();           //calibration routine
        break;

      case 'e':
        readEncoderDiagnostics();   //encoder error?
        break;

      case 'y':
        enableTCInterrupts();      //enable closed loop
        break;

      case 'n':
        disableTCInterrupts();      //disable closed loop
        break;

      case 'r':             //new setpoint
        SerialUSB.println("Enter setpoint:");
        while (SerialUSB.available() == 0)  {}
        r = SerialUSB.parseFloat();
        SerialUSB.println(r);
        break;

      case 'x':
        mode = 'x';           //position loop
        break;

      case 'v':
        mode = 'v';           //velocity loop
        break;

      case 't':
        mode = 't';           //torque loop
        break;

      case 'c':
        mode = 'c';           //custom loop
        break;

      case 'q':
        parameterQuery();     // prints copy-able parameters
        break;

      case 'a':             //anticogging
        antiCoggingCal();
        break;

      case 'k':
        { 
          parameterEditmain();
          
          break;
        }
        case 'f':
        {
            readEncoderNew(0);
            break;
        }
        case 'h':
        {
            readEncoderNew(1);
            break;
        }
        case 'g':
        {
            readEncoder();
            break;  
        }


      default:
        // TODO: change signature of function to return error code and write a mnsg on console
        break;
    }
  }

}
