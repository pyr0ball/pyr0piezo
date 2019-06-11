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

// Debug output toggle. Uncomment to enable
#define DEBUG true

// i2c input toggle. Uncomment to enable
//#define I2C true
#ifdef I2C
	#include <Wire.h>
	#include "pP_i2c.h"
#endif

// Headers, variables, and functions
#include "pP_pins.h"
#include "pP_volatile.h"
#include "pP_functions.cpp"

// Configurable settings:
int GAIN_FACTOR = 2;           // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define InitCount 6             // Number of times to blink the LED on start
int LOOP_DUR = 100;			  // duration of time between ADC checks and other loop functions
int TRG_DUR = 20;             // duration of the Z-axis pulse sent, in ms
#define senseThrs 1.85
#define compThrs 2.54

int Hyst = 20;                 // Hysteresis value for ADC measurements
#define Vin 5                   // input reference voltage


/*------------------------------------------------*/

void loop() {
  
  // Blink LED's on init
  if (BlinkCount > 0) {
	BlinkState = !BlinkState;
	digitalWrite(ERR_LED, BlinkState);
	digitalWrite(TRG_OUT, BlinkState);
	delay(LOOP_DUR);
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
  
  VAdj = analogRead(V_FOLLOW_PIN);
  diffAdjL = ((VAdj - senseInt) / 4) - Hyst;
  diffAdjH = ((senseInt - VAdj) / 4) - Hyst;

  
  // Set the amplification gain factor
  adjustGain();
  
  // Voltage Follower adjustment
  adjustFollow();

  // Voltage Comparator adjustment
  adjustComp();
  
  // Alert the user that auto-calibration is ongoing
  calibrateAlert();
  
  // Check for error state
  checkError();
  
  // Reply with status
  serialReply();
  
  // Sets trigger output state to false after completing loop
  delay(LOOP_DUR);
  digitalWrite(TRG_OUT, HIGH);
  sensorHReading = 0;
}
