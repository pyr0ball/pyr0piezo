#ifndef PP_VOLATILE_H
#define PP_VOLATILE_H

#include "LightChrono.h"
#include "stdint.h"

// these variables will change on their own. Do not edit ANYTHING below this line
extern volatile int sensorHReading;      // variable to store the value read from the sensor pin
extern volatile int ADJ_FOLLOW;          // Variable for Follower adjustment
extern volatile int ADJ_COMP;            // Variable for Comparator adjustment
extern volatile int ERR_STATE;
extern volatile int PZ_STATE;

extern int Vin;                   // input reference voltage in millivolts (multiply V by 1000)
extern int VOld;                  // Variable to store previous cycle's Vin
extern int VLast;

// Convert threshold values based on the input voltage

extern long followerLong;
extern long compLong;
extern long followerInt;
extern long compInt;

// Voltage Comparator Adjustment parameters
extern int VComp;

// Voltage Follower Adjustment parameters
extern int VFol;

// Error blink parameters

extern int BlinkState;
extern int BlinkCount;   // Multiply Blink count by 2 to handle toggle state, add one extra to make sure light is on after

// Serial Input Parsing Variables
#define buffSize 40
extern char inputBuffer[buffSize];
#define endMarker '\n'
extern uint8_t bytesRecvd;
extern bool serialIncoming;
extern char serialMessageIn[buffSize];
extern long serialLong;

//#define LOW 0
//#define HIGH 1

// Task scheduler instances
extern LightChrono mainLoop;

#endif //PP_VOLATILE_H