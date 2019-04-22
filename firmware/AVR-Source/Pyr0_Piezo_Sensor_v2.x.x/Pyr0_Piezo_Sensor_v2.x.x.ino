/*
  Piezoelectric Z-Axis sensor using AtMega88/168/328 (AtMega 48 doesnt have enough memory for this version)

  This sketch reads a piezo element to detect a touch of the printer's nozzle to the bed.
  The sense pin is tied to an interrupt, which is pulled high by internal pullup resistor.
  When the piezo touches the bed, the amplification circuit will draw the interrupt pin low
  and the atmega will output a pulse based on the programmed trigger duration
  
 * PD2 INT0 (Piezo In 'D2')
 * D7 PCINT23 (Trigger OUT 'D7')
 * PC0 ADC0 (Voltage Reference Check 'A0')
 * PC1 ADC1 (Sensitivity Adjustment Check 'A1')
 * PD4 PCINT20 (Error feedback LED 'D4')
 * PB6 PCINT6 (Voltage Adjustment Resistor 0 'D20')
 * PB7 PCINT7 (Voltage Adjustment Resistor 1 'D21')
 * PD5 T1 (Voltage Adjustment Resistor 2 'D5')
 * PD6 PCINT22 (Voltage Adjustment Resistor 3 'D6')
 * PB1 OC1A (Comparator VRef PWM Out 'D9')
 * PD3 OC2B (Voltage Follower VRef PWM Out 'D3')

  Schematics for this project can be found here: https://github.com/pyr0ball/pyr0piezo/tree/master/docs/Schematics

  For Arduino IDE use MCUdude MiniCore: https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json


  created 2/18/2019
  by Alan "pyr0ball" Weinstock

  This code is in the public domain.
*/

/* To set the below parameters using serial input, use the following:

To change trigger active duration: TRG_D [integer for milliseconds]
To change gain factor: GAIN_F [integer for gain state - see note*]
To change ADC hysteresis value: HYST [integer]
To change sensor input pullup vRef low threshold: VADJ [float value]
To change comparator trigger high threshold: VCOMP [float value]


These commands should be wrapped in this format: 
<CMD, INT, FLOAT>

You must include the unused variable for each instance. 

Examples:
<GAIN_F, 3, 0.00>
<VADJ, 0, 2.35>

*Note for Gain Factor:
The gain STATE is representative of these values:
0 = 3x
1 = 3.5x
2 = 4.33x
3 = 6x
4 = 11x
*/

//#include <Wire.h>
 
// Set variables for working parameters
int GAIN_FACTOR = 2;           // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define InitCount 6             // Number of times to blink the LED on start
int TRG_DUR = 120;             // duration of the Z-axis pulse sent, in ms
#define senseThrs 2.45
//float senseHighThrs = 2.35;    // Upper threshold of Voltage Follower before adjustment
//float senseLowThrs = 1.8;      // Lower threshold of Voltage Follower before adjustment
#define compThrs 3.15
//float compHighThrs = 2.75;     // Upper threshold of Comparator before adjustment
//float compLowThrs = 2.54;      // Lower threshold of Comparator before adjustment

#define Hyst 20                 // Hysteresis value for ADC measurements
#define Vin 5                   // input reference voltage


// Analog Pin Assignments
#define V_FOLLOW_PIN A0         // Sense pin to check Voltage Follower stage
#define VCOMP_SENSE_PIN A1      // Sense pin to check comparator stage voltage

// Digital Pin Assignments
#define TRG_OUT 7         // LED and Z-Min trigger output connected to digital pin 7
//#define TRG_OUT 13      // For testing on Atmega328/2560, Output is moved to onboard LED pin
//#define Z_TRG 0              // the piezo is connected to INT0 / digital pin 2
#define Z_TRG 2          // the piezo is connected to INT0 / digital pin 2
#define ERR_LED 4               // LED will blink if optimal voltage range cannot be achieved
#define GADJ_R0 20        // Auto-adjust ladder pin assignments
#define GADJ_R1 21        // "
#define GADJ_R2 5         // "
#define GADJ_R3 6         // "
#define V_FOL_PWM 3             // PWM analog output pin for voltage follower adjustment
#define VCOMP_PWM 9             // PWM analog output pin for comparator adjustment

// these variables will change on their own. Do not edit ANYTHING below this line
volatile int sensorHReading = 0;      // variable to store the value read from the sensor pin
volatile int ADJ_FOLLOW = 0;          // Variable for Follower adjustment
volatile int ADJ_COMP = 0;            // Variable for Comparator adjustment
volatile int ERR_STATE = 0;

// Convert float to integer for adjustment functions
int senseInt = (senseThrs / 5) * 1024;    // Voltage Follower upper converted to adg interger
//int senseHighInt = (senseHighThrs / 5) * 1024;    // Voltage Follower upper converted to adg interger
//int senseLowInt = (senseLowThrs / 5) * 1024;      // Voltage Follower lower converted to adg interger
int compInt = (compThrs / 5) * 1024;      // Upper threshold of Comparator before adjustment
//int compHighInt = (compHighThrs / 5) * 1024;      // Upper threshold of Comparator before adjustment
//int compLowInt = (compLowThrs / 5) * 1024;        // Lower threshold of Comparator before adjustment

// Voltage Comparator Adjustment parameters
//float VCompRef = 0.00;                    // variable to store the float value read from the comparator reference
int VComp = 0;
int diffCompL = VComp - compInt;
int diffCompH = compInt - VComp;
//int diffCompL = VComp - compLowInt;
//int diffCompH = compHighInt - VComp;


// Voltage Follower Adjustment parameters
//float vAdjRead = 0.00;                    // variable to store the value read from the follower
int VAdj = 0;                    
int diffAdjL = VAdj - senseInt;
int diffAdjH = senseInt - VAdj;
//int diffAdjL = VAdj - senseLowInt;
//int diffAdjH = senseHighInt - VAdj;

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

void setup() {
  pinMode(TRG_OUT, OUTPUT); // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(Z_TRG, INPUT_PULLUP); // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VCOMP_SENSE_PIN, INPUT);
  pinMode(GADJ_R0, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R1, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R2, INPUT); // declare input to break pull to ground
  pinMode(GADJ_R3, INPUT); // declare input to break pull to ground
  Serial.begin(9600);

  // Uncomment the following lines to use PCInt pins instead of hardware interrupt
  //#include <PinChangeInterrupt.h>
  //attachPCINT(digitalPinToPCINT(Z_TRG), pulse, FALLING);

  // Uncomment the followoing line to use hardware interrupt pin
  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);

  Serial.println("Initializing Pyr0-Piezo Sensor...");
  
}

/*------------------------------------------------*/

void pulse() {
  digitalWrite(TRG_OUT, LOW);
  sensorHReading = 1;
  delay(TRG_DUR);
}

/*------------------------------------------------*/

void adjustFollow() {
  /* Compares diffs of threshold vs read value
   if positive, adjusts the follower to within
   the range set above*/
  if (diffAdjL > 0.0) {
	ADJ_FOLLOW += diffAdjL;
  }
  if (diffAdjH > 0.0) {
	ADJ_FOLLOW -= diffAdjH;
  }

  // Analog output (PWM) of duty cycle
  analogWrite(V_FOL_PWM, ADJ_FOLLOW);
}

/*------------------------------------------------*/

void adjustComp() {
  if (diffCompL > 0.0) {
	ADJ_COMP += diffCompL;
  }
  if (diffCompH > 0.0) {
	ADJ_COMP -= diffCompH;
  }

  analogWrite(VCOMP_PWM, ADJ_COMP);
}

/*------------------------------------------------*/

void adjustGain() {

  if  (GAIN_FACTOR < 0 || GAIN_FACTOR > 4) {
    ERR_STATE = 1;
  }
  else if (GAIN_FACTOR == 0) {
    pinMode(GADJ_R3, INPUT);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 0) {
    pinMode(GADJ_R3, OUTPUT);
    digitalWrite(GADJ_R3, LOW);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 1) {
    pinMode(GADJ_R2, OUTPUT);
    digitalWrite(GADJ_R2, LOW);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 2) {
    pinMode(GADJ_R1, OUTPUT);
    digitalWrite(GADJ_R1, LOW);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  else if (GAIN_FACTOR > 3) {
    pinMode(GADJ_R0, OUTPUT);
    digitalWrite(GADJ_R0, LOW);
    ERR_STATE = 0;
  }
}

/*------------------------------------------------*/

void checkError () {
  if (ERR_STATE == 1) {
    digitalWrite(ERR_LED, BlinkState);
    BlinkState = !BlinkState;
  }
  else if (ERR_STATE == 0) {
    BlinkState = LOW;
    digitalWrite(ERR_LED, BlinkState);
  }
}

/*------------------------------------------------*/

void serialInput() {

  // receive data from Serial and save it into inputBuffer
  
  if(Serial.available() > 0) {

    // the order of these IF clauses is significant
    identifyMarkers();

  }
}

/*------------------------------------------------*/

/* void i2cInput() {

  // receive data from Serial and save it into inputBuffer
  
  while(Wire.available()) {
    identifyMarkers();
    updateParams();
    i2cReply();
  }
}
*/
/*------------------------------------------------*/

void identifyMarkers() {
  
  char x = Serial.read();
//  char y = Wire.read();

  if (x == endMarker) {
    readInProgress = false;
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
  }

  else if(readInProgress) {
    inputBuffer[bytesRecvd] = x;
    bytesRecvd ++;
    if (bytesRecvd == buffSize) {
      bytesRecvd = buffSize - 1;
    }
  }

  else if (x == startMarker) { 
    bytesRecvd = 0; 
    readInProgress = true;
  }
  
/*  if (y == endMarker) {
    readInProgress = false;
    serialIncoming = true;
    inputBuffer[bytesRecvd] = 0;
    parseData();
  }

  if(readInProgress) {
    inputBuffer[bytesRecvd] = y;
    bytesRecvd ++;
    if (bytesRecvd == buffSize) {
    bytesRecvd = buffSize - 1;
    }
  }

  if (y == startMarker) { 
    bytesRecvd = 0; 
    readInProgress = true;
  }
 */
}

/*------------------------------------------------*/

void parseData() {

  // split the data into its parts
  
  char * strtokIndx; // this is used by strtok() as an index
  
  strtokIndx = strtok(inputBuffer,",");      // get the first part - the string
  strcpy(serialMessageIn, strtokIndx); // copy it to serialMessageIn
  
  strtokIndx = strtok(NULL, ",");   // this continues where the previous call left off
  serialInt = atoi(strtokIndx);     // convert this part to an integer
   
  strtokIndx = strtok(NULL, ","); 
  serialFloat = atof(strtokIndx);     // convert this part to a float

}

/*------------------------------------------------*/

void updateParams() {
  if (strcmp(serialMessageIn, "TRG_D") == 0) {
    updateTrigDuration();
  }
  else if (strcmp(serialMessageIn, "GAIN_F") == 0) {
    updateGainFactor();
  }
  else if (strcmp(serialMessageIn, "VCOMP") == 0) {
    updateVComp();
  }
  else if (strcmp(serialMessageIn, "VADJ") == 0) {
    updateVAdj();
  }
}
/*------------------------------------------------*/

void updateTrigDuration() {
  if (serialInt >= 0) {
    TRG_DUR = serialInt;
  }
}
/*------------------------------------------------*/

void updateGainFactor() {
  if (serialInt >= 0) {
    GAIN_FACTOR = serialInt;
  }
}
/*------------------------------------------------*/

void updateVComp() {
  if (serialInt >= 0) {
    compInt = (serialFloat / 5) * 1024;
  }
}
/*------------------------------------------------*

void updateVCompH() {
  if (serialInt >= 0) {
    compHighThrs = ((float)serialFloat);
  }
}
*------------------------------------------------*

void updateVCompL() {
  if (serialInt >= 0) {
    compLowThrs = ((float)serialFloat);
  }
}
*------------------------------------------------*/

void updateVAdj() {
  if (serialInt >= 0) {
    senseInt = (serialFloat / 5) * 1024;
  }
}
/*------------------------------------------------*

void updateVAdjH() {
  if (serialInt >= 0) {
    senseHighThrs = ((float)serialFloat);
  }
}
*------------------------------------------------*

void updateVAdjL() {
  if (serialInt >= 0) {
    senseLowThrs = ((float)serialFloat);
  }
}
*------------------------------------------------*/

void updateHysteresis() {
  if (serialInt >= 0) {
    Hyst = serialInt;
  }
}
/*------------------------------------------------*/

void serialReply() {
  if (serialIncoming) {
    serialIncoming = false;

    Serial.print("Comp Reference:");
    Serial.print(VComp);
    Serial.print(" ");
    Serial.print("Comparator State:");
    Serial.print(ADJ_COMP);
    Serial.print(" ");
    Serial.println(compInt);

    Serial.print("Diff");
    Serial.print(" ");
    Serial.print(diffCompL);
    Serial.print(" ");
    Serial.println(diffCompH);
  
    Serial.print("Amp Sense:");
    Serial.print(VAdj);
    Serial.print(" ");
    Serial.print("Follower State:");
    Serial.print(ADJ_FOLLOW);
    Serial.print(" ");
    Serial.println(senseInt);

    Serial.print("Diff");
    Serial.print(" ");
    Serial.print(diffAdjL);
    Serial.print(" ");
    Serial.println(diffAdjH);
  
	  Serial.print("Delay:");
	  Serial.println(TRG_DUR);
	  Serial.print("Error State:");
	  Serial.println(ERR_STATE);
	  Serial.println("------------------");
  }
}
/*------------------------------------------------*/
/*
void i2cReply() {
  if (serialIncoming) {
    Wire.write("OK");
  }
}
*/
/*------------------------------------------------*/

void loop() {
  
  // Blink LED's on init
  if (BlinkCount > 0) {
    BlinkState = !BlinkState;
    digitalWrite(ERR_LED, BlinkState);
    digitalWrite(TRG_OUT, BlinkState);
    delay(TRG_DUR);
    --BlinkCount;
  }

  // Get Serial Input
  serialInput();
  
  // Set any new parameters from serial input
  updateParams();
  
  // Check voltage of first and second stages and compare against thresholds
  VComp = analogRead(VCOMP_SENSE_PIN);
  diffCompL = ((VComp - compInt) / 4) - Hyst;
  diffCompH = ((compInt - VComp) / 4) - Hyst;
  //diffCompL = VComp - compLowInt;
  //diffCompH = compHighInt - VComp;
  //VCompRef = (float)(VComp * 5) / 1024;
  
  VAdj = analogRead(V_FOLLOW_PIN);
  diffAdjL = ((VAdj - senseInt) / 4) - Hyst;
  diffAdjH = ((senseInt - VAdj) / 4) - Hyst;
  //diffAdjL = VAdj - senseLowInt;
  //diffAdjH = senseHighInt - VAdj;
  //vAdjRead = (float)(VAdj * 5) / 1024;

  
  // Set the amplification gain factor
  adjustGain();
  
  // Voltage Follower adjustment
  adjustFollow();

  // Voltage Comparator adjustment
  adjustComp();
  
  // Check for error state
  checkError();
  
  // Reply with status
  serialReply();
  
  // Sets trigger output state to false after completing loop
  delay(TRG_DUR);
  digitalWrite(TRG_OUT, HIGH);
  sensorHReading = 0;
}
