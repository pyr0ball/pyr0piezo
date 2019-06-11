// these variables will change on their own. Do not edit ANYTHING below this line
volatile int sensorHReading = 0;      // variable to store the value read from the sensor pin
volatile int ADJ_FOLLOW = 0;          // Variable for Follower adjustment
volatile int ADJ_COMP = 0;            // Variable for Comparator adjustment
volatile int ERR_STATE = 0;

// Convert float to integer for adjustment functions
int senseInt = (senseThrs * 1024) / Vin;    // Voltage Follower upper converted to adg interger
int compInt = (compThrs * 1024) / Vin;      // Upper threshold of Comparator before adjustment

// Voltage Comparator Adjustment parameters
//float VCompRef = 0.00;                    // variable to store the float value read from the comparator reference
int VComp = 0;
int diffCompL = VComp - compInt;
int diffCompH = compInt - VComp;

// Voltage Follower Adjustment parameters
//float vAdjRead = 0.00;                    // variable to store the value read from the follower
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
float serialFloat = 0.00;