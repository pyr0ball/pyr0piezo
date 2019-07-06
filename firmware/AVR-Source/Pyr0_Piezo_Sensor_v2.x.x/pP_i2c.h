#ifdef I2C
#ifndef _pP_i2c_h_
#define _pP_i2c_h_

#include <Wire.h>

#define pP_i2c_address  0xa0 // I2C Bus Address

#define senseInt_Offset 0x00 // Integer of sense threshold in millivolts
#define compInt_Offset  0x01 // Integer of comparator threshold in millivolts
#define gainFactor_Offset 0x02  // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define hysteresis_Offset 0x03  // Hysteresis value for ADC measurements
#define loopDuration_Offset 0x04  // duration of time between ADC checks and other loop functions
#define triggerDuration_Offset  0x05 // duration of the Z-axis pulse sent, in ms
#define voltMeterLong_Offset 0x06  // For fine-tuning the input volt master

/*-------------------------Variables------------------------*/
#define regMapSize  7
#define maxBytes  2
#define longBytes 4
byte regMap[regMapSize];
byte regMapTemp[regMapSize];
byte cmdRcvd[maxBytes];
byte longRcvd[longBytes];

/*------------------------------------------------*/

class pP_i2c {
  public:
    pP_i2c(uint8_t address=pP_i2c_address);

    void init();
    void i2cInput();
};

#endif
#endif
