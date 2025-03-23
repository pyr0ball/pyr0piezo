#include "pP_volatile.h"
#include "pP_config.h"
#include "stdint.h"

// these variables will change on their own. Do not edit ANYTHING below this line
volatile int ADJ_FOLLOW = 0;     // Variable for Follower adjustment
volatile int ADJ_COMP = 0;       // Variable for Comparator adjustment
volatile int ERR_STATE = 0;
volatile int PZ_STATE = 0;

int Vin = 5000;  // input reference voltage in millivolts (multiply V by 1000)
int VOld = 5000; // Variable to store previous cycle's Vin
int VLast = 0;

// Convert threshold values based on the input voltage

long followerLong = followerThrs * 1023L;
long compLong = compThrs * 1023L;
long followerInt = 0;
long compInt = 0;

// Voltage Comparator Adjustment parameters
int VComp = 0;

// Voltage Follower Adjustment parameters
int VFol = 0;

// Error blink parameters

int BlinkCount = InitCount * 2; // Multiply Blink count by 2 to handle toggle state

// Serial Input Parsing Variables
char inputBuffer[buffSize];
uint8_t bytesRecvd = 0;
bool serialIncoming = false;
char serialMessageIn[buffSize] = {0};
long serialLong = 0;

long lastTriggerTimestamp = 0;

LightChrono mainLoop;
LightChrono blinkLoop;