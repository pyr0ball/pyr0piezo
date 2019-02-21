/*
  Piezoelectric Z-Axis sensor using AtMega88/168/328 (AtMega 48 doesnt have enough memory for this version)

  This sketch reads a piezo element to detect a touch of the printer's nozzle to the bed.
  The sense pin is tied to an interrupt, which is pulled high by internal pullup resistor.
  When the piezo touches the bed, the amplification circuit will draw the interrupt pin low
  and the atmega will output a pulse based on the programmed trigger duration
  
 * PD2 INT0 (Piezo In 'D2')
 * PB1 PCINT1 (Trigger OUT 'D9')
 * PC0 ADC0 (Voltage Reference Check 'A0')
 * PC1 ADC1 (Sensitivity Adjustment Check 'A1')
 * PD3 INT1 (Error feedback LED 'D3')
 * PB6 PCINT6 (Voltage Adjustment Resistor 0 'D20')
 * PB7 PCINT7 (Voltage Adjustment Resistor 1 'D21')
 * PD5 T1 (Voltage Adjustment Resistor 2 'D5')
 * PD6 PCINT22 (Voltage Adjustment Resistor 3 'D6')

  created 1/8/2019
  by Alan "pyr0ball" Weinstock

  This code is in the public domain.
  Based off examples from:
  http://www.arduino.cc/en/Tutorial/Knock
  http://www.circuitbasics.com/arduino-ohm-meter/
*/


// Set variables for working parameters
const int TRG_OUT = 9;         // LED and Z-Min trigger output connected to digital pin 9
//const int TRG_OUT = 13;      // For testing on Atmega328/2560, Output is moved to onboard LED pin
//#define Z_TRG 0              // the piezo is connected to INT0 / digital pin 2
const byte Z_TRG = 2;          // the piezo is connected to INT0 / digital pin 2
int ERR_LED = 3;               // LED will blink if optimal voltage range cannot be achieved
int InitCount = 6;             // Number of times to blink the LED on start
int V_FOLLOW_PIN = A0;             // Sense pin to check first amp stage voltage output
int VADJ_SENSE_PIN = A1;           // Sense pin to check comparator stage voltage
int TRG_DUR = 120;             // duration of the Z-axis pulse sent, in ms
int Vin = 5;                   // input reference voltage
float senseHighThrs = 2.35;    // Upper threshold voltage of amp circuit before adjustment
float senseLowThrs = 1.8;     // Lower threshold voltage of amp circuit before adjustment
const int VADJ_R0 = 20;        // Auto-adjust ladder pin assignments
const int VADJ_R1 = 21;
const int VADJ_R2 = 5;
const int VADJ_R3 = 6;

// these variables will change on their own. Do not edit them
volatile int sensorHReading = 0;      // variable to store the value read from the sensor pin
volatile int ADJ_STATE = 0;             // Variable for voltage adjustment ladder
volatile int ERR_STATE = 0;
int folRead = 0;
float VRef = 0.00;                 // variable to store the value read from the reference voltage
int vadjRead = 0;
float VAdj = 0.00;                    // variable to store the value read from the second amp stage
int BlinkState = LOW;
int BlinkCount = InitCount * 2; // Multiply Blink count by 2 to handle toggle state
float diffAdjL = VAdj - senseLowThrs;
float diffAdjH = senseHighThrs - VAdj;

void setup() {
  pinMode(TRG_OUT, OUTPUT); // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(Z_TRG, INPUT_PULLUP); // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VADJ_SENSE_PIN, INPUT);
  pinMode(VADJ_R0, INPUT); // declare input to break pull to ground
  pinMode(VADJ_R1, INPUT); // declare input to break pull to ground
  pinMode(VADJ_R2, INPUT); // declare input to break pull to ground
  pinMode(VADJ_R3, INPUT); // declare input to break pull to ground
  Serial.begin(9600);

  // Uncomment the following lines to use PCInt pins instead of hardware interrupt
  //#include <PinChangeInterrupt.h>
  //attachPCINT(digitalPinToPCINT(Z_TRG), pulse, FALLING);

  // Uncomment the followoing line to use hardware interrupt pin
  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);

  Serial.println("Initializing Piezo Sensor...");
  
}

void pulse() {
  digitalWrite(TRG_OUT, LOW);
  sensorHReading = 1;
}

void adjustState() {
  
  if (diffAdjL > 0.0) {
    ADJ_STATE++;
  }
  
  if (diffAdjH > 0.0) {
    ADJ_STATE--;
  }
  Serial.print("adjustState:");
  Serial.println(ADJ_STATE);
  Serial.println("--------------------");
}

void adjustVoltage() {

  if  (ADJ_STATE < 0) {
    ERR_STATE = 1;
  }
  if (ADJ_STATE == 0) {
    pinMode(VADJ_R3, INPUT);
    pinMode(VADJ_R2, INPUT);
    pinMode(VADJ_R1, INPUT);
    pinMode(VADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (ADJ_STATE > 0) {
    pinMode(VADJ_R3, OUTPUT);
    digitalWrite(VADJ_R3, LOW);
    pinMode(VADJ_R2, INPUT);
    pinMode(VADJ_R1, INPUT);
    pinMode(VADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (ADJ_STATE > 1) {
    pinMode(VADJ_R2, OUTPUT);
    digitalWrite(VADJ_R2, LOW);
    pinMode(VADJ_R1, INPUT);
    pinMode(VADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (ADJ_STATE > 2) {
    pinMode(VADJ_R1, OUTPUT);
    digitalWrite(VADJ_R1, LOW);
    pinMode(VADJ_R0, INPUT);
    ERR_STATE = 0;
  }
  if (ADJ_STATE > 3) {
    pinMode(VADJ_R0, OUTPUT);
    digitalWrite(VADJ_R0, LOW);
    ERR_STATE = 0;
  }
  if  (ADJ_STATE > 4) {
    ERR_STATE = 1;
  }
}

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
  folRead = analogRead(V_FOLLOW_PIN);
  VRef = (folRead * 5.0) / 1024.0;
  vadjRead = analogRead(VADJ_SENSE_PIN);
  VAdj = (vadjRead * 5.0) / 1024.0;
  diffAdjL = VAdj - senseLowThrs;
  diffAdjH = senseHighThrs - VAdj;
  
  // Voltage state adjustment
  adjustState();

  // Voltage divider adjustment
  adjustVoltage();
  
  // Check for error state
  checkError();

  // Checks state of the interrupt trigger, prints status, then sets output pin low
    Serial.print("PZ Status:");
    Serial.println(sensorHReading);
    Serial.print("Voltage Reference:");
    Serial.println(VRef);
    Serial.print("Amp Sense:");
    Serial.println(VAdj);
    Serial.print("Adjustment State:");
    Serial.println(ADJ_STATE);
    Serial.print("Delay:");
    Serial.println(TRG_DUR);
    Serial.print("Low Diff:");
    Serial.println(diffAdjL);
    Serial.print("High Diff:");
    Serial.println(diffAdjH);
    Serial.print("Error State:");
    Serial.println(ERR_STATE);
    Serial.println("------------------");
    delay(TRG_DUR);
    digitalWrite(TRG_OUT, HIGH);
    sensorHReading = 0;
}
