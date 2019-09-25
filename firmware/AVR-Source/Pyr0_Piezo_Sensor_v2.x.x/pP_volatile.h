// these variables will change on their own. Do not edit ANYTHING below this line
volatile int sensorHReading = 0;      // variable to store the value read from the sensor pin
volatile int ADJ_FOLLOW = 0;          // Variable for Follower adjustment
volatile int ADJ_COMP = 0;            // Variable for Comparator adjustment
volatile int ERR_STATE = 0;

int Vin = 5000;                   // input reference voltage in millivolts (multiply V by 1000)
int VOld = 5000;                  // Variable to store previous cycle's Vin
int VLast = 0;

// Convert threshold values based on the input voltage

long senseLong = senseThrs * 1024L;
long compLong = compThrs * 1024L;
long senseInt;
long compInt;

// Voltage Comparator Adjustment parameters
int VComp = 0;

// Voltage Follower Adjustment parameters
int VAdj = 0;
int diffAdjL = VAdj - senseInt;
int diffAdjH = senseInt - VAdj;

// Error blink parameters

int BlinkState = LOW;
int BlinkCount = InitCount * 2;           // Multiply Blink count by 2 to handle toggle state

// Serial Input Parsing Variables
#define buffSize 40
char inputBuffer[buffSize];
#define startMarker '<'
#define endMarker '>'
byte bytesRecvd = 0;
bool readInProgress = false;
bool serialIncoming = false;
char serialMessageIn[buffSize] = {0};
int serialInt = 0;

//#define LOW 0
//#define HIGH 1

// Task scheduler instances
LightChrono mainLoop;
