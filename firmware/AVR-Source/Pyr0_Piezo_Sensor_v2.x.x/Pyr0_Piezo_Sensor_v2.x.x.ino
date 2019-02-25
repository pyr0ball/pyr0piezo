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

// Set variables for working parameters
int GAIN_FACTOR = 2;           // Gain adjustment factor. 0=2x, 1=2.5x, 2=3.33x, 3=5x, 4=10x
int InitCount = 6;             // Number of times to blink the LED on start
int TRG_DUR = 120;             // duration of the Z-axis pulse sent, in ms
float senseHighThrs = 2.35;    // Upper threshold of Voltage Follower before adjustment
float senseLowThrs = 1.8;      // Lower threshold of Voltage Follower before adjustment
float compHighThrs = 2.75;     // Upper threshold of Comparator before adjustment
float compLowThrs = 2.54;      // Lower threshold of Comparator before adjustment
int Vin = 5;                   // input reference voltage

// Analog Pin Assignments
int V_FOLLOW_PIN = A0;         // Sense pin to check Voltage Follower stage
int VCOMP_SENSE_PIN = A1;      // Sense pin to check comparator stage voltage

// Digital Pin Assignments
const int TRG_OUT = 7;         // LED and Z-Min trigger output connected to digital pin 7
//const int TRG_OUT = 13;      // For testing on Atmega328/2560, Output is moved to onboard LED pin
//#define Z_TRG 0              // the piezo is connected to INT0 / digital pin 2
const byte Z_TRG = 2;          // the piezo is connected to INT0 / digital pin 2
int ERR_LED = 4;               // LED will blink if optimal voltage range cannot be achieved
const int GADJ_R0 = 20;        // Auto-adjust ladder pin assignments
const int GADJ_R1 = 21;        // "
const int GADJ_R2 = 5;         // "
const int GADJ_R3 = 6;         // "
int V_FOL_PWM = 3;             // PWM analog output pin for voltage follower adjustment
int VCOMP_PWM = 9;             // PWM analog output pin for comparator adjustment

// these variables will change on their own. Do not edit ANYTHING below this line
volatile int sensorHReading = 0;      // variable to store the value read from the sensor pin
volatile int ADJ_FOLLOW = 0;          // Variable for Follower adjustment
volatile int ADJ_COMP = 0;            // Variable for Comparator adjustment
volatile int ERR_STATE = 0;

// Convert float to integer for adjustment functions
int senseHighInt = (senseHighThrs / 5) * 1024;    // Voltage Follower upper converted to adg interger
int senseLowInt = (senseLowThrs / 5) * 1024;      // Voltage Follower lower converted to adg interger
int compHighInt = (compHighThrs / 5) * 1024;      // Upper threshold of Comparator before adjustment
int compLowInt = (compLowThrs / 5) * 1024;        // Lower threshold of Comparator before adjustment

// Voltage Comparator Adjustment parameters
float VCompRef = 0.00;                    // variable to store the float value read from the comparator reference
int VComp = 0;
int diffCompL = VComp - compLowInt;
int diffCompH = compHighInt - VComp;


// Voltage Follower Adjustment parameters
float vAdjRead = 0.00;                    // variable to store the value read from the follower
int VAdj = 0;                    
int diffAdjL = VAdj - senseLowInt;
int diffAdjH = senseHighInt - VAdj;

// Error blink parameters
int BlinkState = LOW;
int BlinkCount = InitCount * 2;           // Multiply Blink count by 2 to handle toggle state

// Serial Input Parsing Variables
String readString, substring;
int loc; 


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

  Serial.println("Initializing Piezo Sensor...");
  
}

/*------------------------------------------------*/

void pulse() {
  digitalWrite(TRG_OUT, LOW);
  sensorHReading = 1;
}

/*------------------------------------------------*/

void adjustFollow() {
  /* Compares diffs of threshold vs read value
   if positive, adjusts the follower to within
   the range set above*/
  if (diffAdjL > 0.0) {
    ADJ_FOLLOW + (diffAdjL / 4);
  }
  if (diffAdjH > 0.0) {
    ADJ_FOLLOW - (diffAdjH / 4);
  }

  // Analog output (PWM) of duty cycle
  analogWrite(V_FOL_PWM, ADJ_FOLLOW);
  
  Serial.print("Follower State:");
  Serial.println(ADJ_FOLLOW);
  Serial.println("--------------------");
}

/*------------------------------------------------*/

void adjustComp() {
  if (diffCompL > 0.0) {
    ADJ_COMP + (diffCompL / 4);
  }
  
  if (diffCompH > 0.0) {
    ADJ_COMP - (diffCompH / 4);
  }

  analogWrite(VCOMP_PWM, ADJ_COMP);
  
  Serial.print("Comparator State:");
  Serial.println(ADJ_COMP);
  Serial.println("--------------------");
}

/*------------------------------------------------*/

void adjustGain() {

  if  (GAIN_FACTOR < 0) {
    ERR_STATE = 1;
  }
  if (GAIN_FACTOR == 0) {
    pinMode(GADJ_R3, INPUT);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (GAIN_FACTOR > 0) {
    pinMode(GADJ_R3, OUTPUT);
    digitalWrite(GADJ_R3, LOW);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (GAIN_FACTOR > 1) {
    pinMode(GADJ_R2, OUTPUT);
    digitalWrite(GADJ_R2, LOW);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (GAIN_FACTOR > 2) {
    pinMode(GADJ_R1, OUTPUT);
    digitalWrite(GADJ_R1, LOW);
    pinMode(GADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (GAIN_FACTOR > 3) {
    pinMode(GADJ_R0, OUTPUT);
    digitalWrite(GADJ_R0, LOW);
    ERR_STATE = 0;
  }
  if  (GAIN_FACTOR > 4) {
    ERR_STATE = 1;
  }
}

/*------------------------------------------------*/

void checkError () {
  if (ERR_STATE == 1) {
    digitalWrite(ERR_LED, BlinkState);
    BlinkState = !BlinkState;
  }
  if (ERR_STATE == 0) {
    BlinkState = LOW;
    digitalWrite(ERR_LED, BlinkState);
  }
}

/*------------------------------------------------*/

void serialInput() {

  //expect a string like VFOLH2.35 VFOLL1.80 VCOMPH2.75 VCOMPL2.54 GFACT2

  if (Serial.available())  {
    char c = Serial.read();  //gets one byte from serial buffer
    if (c == '\n') {  //looks for end of data packet marker
    //if (c == ',') {
      Serial.println(readString); //prints string to serial port out


      //Analyze each input
      loc = readString.indexOf("VFOLH");
      //Serial.println(loc);
      if (loc != -1) {  
        substring = readString.substring(loc+5, loc+9);
		if (isFloat(substring)) {
		  char carray[substring.length() + 1]; // determine size of the array
		  substring.toCharArray(carray, sizeof(carray)); // puts substring into an array
		  senseHighThrs = atof(carray);
		}
        Serial.print("VFollowHigh: ");
        Serial.println(substring);
      } else {
        Serial.print("VFollowHigh: ");
        Serial.println("NA");
      }      


      loc = readString.indexOf("VFOLL");
      //Serial.println(loc);
      if (loc != -1) {  
        substring = readString.substring(loc+5, loc+9);
		if (isFloat(substring)) {
		  char carray[substring.length() + 1]; // determine size of the array
		  substring.toCharArray(carray, sizeof(carray)); // puts substring into an array	
		  senseLowThrs = atof(carray);
		}
        Serial.print("VFollowLow: ");
        Serial.println(substring);
      } else {
        Serial.print("VFollowLow: ");
        Serial.println("NA");
      }        
      
      loc = readString.indexOf("VCOMPH");
      //Serial.println(loc);
      if (loc != -1) {  
        substring = readString.substring(loc+6, loc+10);
		if (isFloat(substring)) {
		  char carray[substring.length() + 1]; // determine size of the array
		  substring.toCharArray(carray, sizeof(carray)); // puts substring into an array
		  compHighThrs = atof(carray);
		}
        Serial.print("VCompHigh: ");
        Serial.println(substring);
      } else {
        Serial.print("VCompHigh: ");
        Serial.println("NA");
      } 
	  
      loc = readString.indexOf("VCOMPL");
      //Serial.println(loc);
      if (loc != -1) {  
        substring = readString.substring(loc+6, loc+10);
		if (isFloat(substring)) {
		  char carray[substring.length() + 1]; // determine size of the array
		  substring.toCharArray(carray, sizeof(carray)); // puts substring into an array
		  compLowThrs = atof(carray);
		}
        Serial.print("VCompLow: ");
        Serial.println(substring);
      } else {
        Serial.print("VCompLow: ");
        Serial.println("NA");
      } 
	  
	  loc = readString.indexOf("GFACT");
      //Serial.println(loc);
      if (loc != -1) {  
        substring = readString.substring(loc+5, loc+9);
		char carray[substring.length() + 1]; // determine size of the array
		substring.toCharArray(carray, sizeof(carray)); // puts substring into an array
		GAIN_FACTOR = atof(carray);
        Serial.print("GainFactor: ");
        Serial.println(substring);
      } else {
        Serial.print("GainFactor: ");
        Serial.println("NA");
      } 
        
      readString=""; //clears variable for new input
      substring=""; 

    }  
    else {     
      readString += c; //makes the string readString
    }        
  }
  
}

// Routine for checking if string is a valid number
boolean isFloat(String tString) {
  String tBuf;
  boolean decPt = false;
  
  if(tString.charAt(0) == '+' || tString.charAt(0) == '-') tBuf = &tString[1];
  else tBuf = tString;  

  for(int x=0;x<tBuf.length();x++)
  {
    if(tBuf.charAt(x) == '.') {
      if(decPt) return false;
      else decPt = true;  
    }    
    else if(tBuf.charAt(x) < '0' || tBuf.charAt(x) > '9') return false;
  }
  return true;
}


void loop() {
  
  // Blink LED's on init
  if (BlinkCount > 0) {
    BlinkState = !BlinkState;
    digitalWrite(ERR_LED, BlinkState);
    digitalWrite(TRG_OUT, BlinkState);
    delay(150);
    --BlinkCount;
  }

  // Check voltage of first and second stages and compare against thresholds

  VComp = analogRead(VCOMP_SENSE_PIN);
  diffCompL = VComp - compLowInt;
  diffCompH = compHighInt - VComp;
  VCompRef = (VComp * 5) / 1024;
  
  VAdj = analogRead(V_FOLLOW_PIN);
  diffAdjL = VAdj - senseLowInt;
  diffAdjH = senseHighInt - VAdj;
  vAdjRead = (VAdj * 5) / 1024;

  // Set the amplification gain factor
  adjustGain();
  
  // Voltage Follower adjustment
  adjustFollow();

  // Voltage Comparator adjustment
  adjustComp();
  
  // Check for error state
  checkError();

  // Get Serial Input
  serialInput();

  // Checks state of the interrupt trigger, prints status, then sets output pin low
    Serial.print("PZ Status:");
    Serial.println(sensorHReading);
    Serial.print("Voltage Reference:");
    Serial.println(VCompRef);
    Serial.print("Amp Sense:");
    Serial.println(vAdjRead);
    Serial.print("Comparator State:");
    Serial.println(ADJ_COMP);
    Serial.print("Follower State:");
    Serial.println(ADJ_FOLLOW);
    Serial.print("Delay:");
    Serial.println(TRG_DUR);
    Serial.print("Error State:");
    Serial.println(ERR_STATE);
    Serial.println("------------------");
    delay(TRG_DUR);
    digitalWrite(TRG_OUT, HIGH);
    sensorHReading = 0;
}
