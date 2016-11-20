#ifndef AS5047D_H
#define AS5047D_H


//  AS5047D
//  14-Bit On-Axis Magnetic Rotary Position Sensor with 11-Bit Decimal and Binary Incremental Pulse Count
//  The AS5047D is a high-resolution rotary position sensor for fast absolute angle measurement over a full 360-degree range. This new position sensor is
//  equipped with revolutionary integrated dynamic angle error compensation (DAECTM) with almost 0 latency and offers a robust design that suppresses the
//  influence of any homogenous external stray magnetic field.
//  A standard 4-wire SPI serial interface allows a host microcontroller to read 14-bit absolute angle position data from the AS5047D and to program non-
//  volatile settings without a dedicated programmer.
//  Incremental movements are indicated on a set of ABI signals with a maximum resolution of 2000 steps / 500 pulses per revolution in decimal mode and 2048
//  steps / 512 pulses per revolution in binary mode. The resolution of the ABI signal is programmable and can be reduced to 32 steps per revolution, or 8
//  pulses per revolution.

//SPI Interface (slave)
//The SPI interface is used by a host microcontroller (master) to read or write the volatile memory as well as to program the non-volatile OTP registers.
//The AS5047D SPI only supports slave operation mode. It communicates at clock rates up to 10 MHz.
//The AS5047D SPI uses mode=1 (CPOL=0, CPHA=1) to exchange data. As shown in Figure 11, a data transfer starts with the falling edge of CSn (SCL is low).
//The AS5047D samples MOSI data on the falling edge of SCL. SPI commands are executed at the end of the frame (rising edge of CSn). The bit order is MSB
//first. Data is protected by parity.



// Volatile Registers
#define AS5047D_NOP         (0x0000)        // No Operation
#define AS5047D_ERRFL       (0x0001)        // Error Register
#define AS5047D_PROG        (0x0003)        // Programming Register
#define AS5047D_DIAAGC      (0x3FFC)        // Diagnostic and AGC
#define AS5047D_MAG         (0x3FFD)        // CORDIC Magnitude ( see https://en.wikipedia.org/wiki/CORDIC for info on CORDIC)
#define AS5047D_ANGLEUNC    (0x3FFE)        // Measured angle without dynamic angle error compensation
#define AS5047D_ANGLECOM    (0x3FFF)        // Measured angle with dynamic angle error compenstation

// Error Register Fields
#define ERRFL_FRERR         (0x1 << 0)      // (R) Framing Error: is set to 1 when a non compliant Frame is detected
#define ERRFL_INVCOMM       (0x1 << 1)      // (R) Invalid Command Error: is set to 1 when reading or writting an invalid register adddress
#define ERRFL_PARERR        (0x1 << 2)      // (R) Parity Error

// Programming Register Fields
#define PROG_PROGEN         (0x1 << 0)      // (R/W) Program OTP ennable: enables programming the entire OTP memory
#define PROG_OTPREF         (0x1 << 1)      // (R/W) Refreshes the non volative memory content with the OTP program content
#define PROG_PROGOTP        (0x1 << 3)      // (R/W) Start OTP programming cycle
#define PROG_PROGVER        (0x1 << 6)      // (R/W) Program verify: must be set to on for verifying the correctness of the OTP programming

// Diagnostic and AGC Fields
#define DIAAGC_AGC_VAL          (0xFF << 0)     // (R) Automatic Gain Control Value
#define DIAAGC_LF           (0x1 << 8)      // (R) Diagnostics Offset compensation LF=0: internal offset loop not ready regulated LF=1: internal offset loop finished
#define DIAAGC_COF          (0x1 << 9)      // (R) Diagnostics: CORDIC overflow
#define DIAAGC_MAGH         (0x1 << 10)     // (R) Diagnostics: Magnetic field strength too high AGC = 0x00
#define DIAAGC_MAGL         (0x1 << 11)     // (R) Diagnostics: Magnetic field strenght tooo low AGC = 0xFF

// CORDIC Magnitude Fields
# define MAG_CMAG           (0x3FFF << 0)  // (R) CORDIC Magnitude information


// Measured angle without comp Fields
#define ANGLEUNC_CORDICANG  (0x3FFF << 0)   // (R) Angle information without dynamic angle error compensation

// Measured angle with comp Fields
#define ANGLE_COM_DAECANG   (0x3FFF << 0)  // (R) Angle information without dynamic angle error compensation


// Non-Volatile Registers (OTP)
// The OTP (One-Time Programmable) memory is used to store the absolute zero position of the sensor and the customer settings permanently in the sensor IC.
// SPI write/read access is possible several
// times for all nonvolatile registers (soft write). Soft written register content will be lost after a hardware reset.
// The programming itself can be done just once.
// Therefore the content of the nonvolatile registers is stored permanently in the sensor.
// The register content is still present after a hardware reset and cannot be overwritten.
// For a correct function of the sensor, the OTP programming is not required.

// Non Volatile Registers
#define AS5047D_ZPOSM       (0x0016)    // Zero position MSB
#define AS5047D_ZPOSL       (0x0017)    // Zero position LSB/MAG Diagnostic
#define AS5047D_SETTINGS1   (0x0018)    // Custom settings register 1
#define AS5047D_SETTINGS2   (0x0019)    // Custom settings register 2

// Zero position MSB Fields
#define ZPOSM_ZPOSM         (0x00FF << 0) // (R/W/P) 8 most significant bits of the zero position


// Zero position LSB and MAG Diagnostics Fields
#define ZPOSL_ZPSOL         (0x003F << 0) // (R/W/P) 6 least significant bits of the zero position
#define ZPOSL_COMPLERR      (0x1 << 6)    // (R/W/P) this bit enables the contribution of the MAGL to the error flag
#define ZPOSL_COMPHERR      (0x1 << 7)    // (R/W/P) this bit enables the contribution of the MAGH to the error flag

// Custom settings register 1 Fields
#define SETTINGS_1_FACTORY  (0x1 << 0)  // (R) Pre-Programmed to 1
#define SETTINGS_1_NOTUSED  (0x1 << 1)  // (R/W/P) Preprogram to 0 must not be overwritten
#define SETTINGS_1_DIR      (0x1 << 2)  // rotation direction
#define SETTINGS_1_UVW_ABI  (0x1 << 3)  // (R/W/P) Defines the PWM output (0=ABI is operating, W is used as PWM 1=UVW is operating is used as PWM)
#define SETTINGS_1_DAECDIS  (0x1 << 4)  // (R/W/P) Disable Dynamic and Error Compensation 0=DAE compensation ON, 1=DAE compensation OFF
#define SETTINGS_1_ABIBIN   (0x1 << 5)  // (R/W/P) ABI decimal or binary selection of the ABI pulses per revolution
#define SETTINGS_1_DATASEL  (0x1 << 6)  // (R/W/P) This bit defines which data can be read from address 16383dec (3FFFhex). 0-> DAECANG 1->CORDICANG
#define SETTINGS_1_PWMON    (0x1 << 7)  // (R/W/P) Enable PWM ( setting of UVW_ABI bit necessary)

// Custom settings register 2 Fields
#define SETTINGS_2_UVWPP    (0x7 << 0) // (R/W/P) UVW number of pole pairs 000=1, 001=2, 010=3, 011=4, 100=5, 101=6, 110=7, 111= 8
#define SETINGGS_2_HYS      (0x3 << 3) // (R/W/P) Hysteresis setting
#define SETTINGS_2_ABIRES   (0x7 << 5) // (R/W/P) Resolution of ABI



// To increase the reliability of communication over the SPI, an even parity bit PARC must be generated and sent.
// A wrong setting of the parity bit causes a parity bit error, which is shown as PARERR bit in the ERRFL register.
// The parity bit is calculated from the lower 15 bits of the command frame.
// The complete 16-bit command frame consists of a register address, read/write bit and the parity bit.

#define PARC        (0x1 << 15)    // Parity bit (even) calculated on the lower 15bits of command frame
#define RW          (0x1 << 14)     // 0: Write 1:Read
#define PARD        (0x1 << 15)    // Parity but (even) for the data frame
#define EF          (0x1 << 14)    // Error Flag 0:No data frame error occurred  1:Error occurred






// Functions used to access the registers of the AS5047D via the SPI interface
// 
// a) Initialize
// a) Read Register
// b) Write Register



// The AS5047D SPI uses mode=1 (CPOL=0, CPHA=1) to exchange data.
// A data transfer starts with the falling edge of CSn (SCL is low).
// The AS5047D samples MOSI data on the falling edge of SCL.
// SPI commands are executed at the end of the frame (rising edge of CSn).
// The bit order is MSB first.
// Data is protected by parity.

void AS5047D_Init();


//The data is sent on the MISO pin.
//The parity bit PARD is calculated by the AS5047D of the lower 15 bits of data frame.
// If an error occurred in the previous SPI command frame, the EF bit is set high.
// The SPI read is sampled on the rising edge of CSn and the data is transmitted on MISO with the next read command.

unsigned short AS5047D_Read_Register( unsigned short register);


// The parity bit PARD must be calculated from the lower 15 bit of write data frame.
// In a SPI write transaction, the write command frame is followed by a write data frame at MOSI.
// The write data frame consists of the new register content, which was addressed is in the previous command frame.
// During data transmission on MOSI, the current register content is send on MISO.
// At the next command on MOSI the updated content of the register is transmitted on MISO,

void AS5047D_Write_Register ( unsigned short register );
































#endif // AS5047D_H
