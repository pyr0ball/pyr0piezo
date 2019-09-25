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
------------------------------------------------------------*/

/*-----------------------------------------------------------
To set the below parameters using serial input, use the following:

To change trigger active duration: TRG_D [integer for milliseconds]
To change gain factor: GAIN_F [integer for gain state - see note*]
To change ADC hysteresis value: HYST [integer]
To change sensor input pullup vRef low threshold: VFOL [integer in millivolts]
To change comparator trigger high threshold: VCOMP [integer in millivolts]
To change the duration between ADC measurements: LOOP_D [integer in milliseconds]
To update the internal vRef constant value **(see notes below): CONST [long value]

You can also enable or disable DEBUG output with: DEBUG [0|1]

You can query the current configuration with: CONFIG
You can query the current state (including ADC measurements) with: STATE

To reset all settings to defaults, use: RESET

These commands should be wrapped in this format:
CMD INT

Examples:
GAIN_F 3 <~ set gain factor to index 3 (6x)
VFOL 2350 <~ set the vref floor to 2.35V
DEBUG 1 <~ Enable debug output
STATE <~ display current sensor status

*Note for Gain Factor:
The gain STATE is representative of these values:
0 = 3x
1 = 3.5x
2 = 4.33x
3 = 6x
4 = 11x

**Note for calibrating internal 1.1v vRef:
The ADC reading function assumes an "ideal" multiplier constant. Each Atmega
chip is slightly different, so it won't be completely accurate without tuning.
Most of the time this won't be necessary, so don't mess with this if you don't
know what you're doing!
The reading can be fine-tuned by using a multimeter, and this equation:

scale_constant = internal1.1Ref * 1023 * 1000

where

internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)

If the scale_constant calculated is different from the default 1125300,
update the voltMeterConstant variable in pP_config.h with the correct value

------------------------------------------------------------*/

// Debug output toggle. Uncomment to enable
#define DEBUG true

/*  Debug output verbose mode will continuously output sensor readings
    rather than waiting for user input */
//#define VERBOSE true

// Headers, variables, and functions
include <Arduino.h>
include <EEPROM.h>
#include "LightChrono.h"
#include "pP_pins.h"
#include "pP_config.h"
#include "pP_volatile.h"
#include "pP_function.h"
#include "pP_serial.h"

// i2c input toggle. Uncomment to enable
//#define I2C_INPUT true

void setup() {
  pinMode(TRG_OUT, OUTPUT);       // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(Z_TRG, INPUT_PULLUP);   // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VCOMP_SENSE_PIN, INPUT);
  pinMode(GADJ_R0, INPUT);        // declare input to set high impedance
  pinMode(GADJ_R1, INPUT);        // declare input to set high impedance
  pinMode(GADJ_R2, INPUT);        // declare input to set high impedance
  pinMode(GADJ_R3, INPUT);        // declare input to set high impedance
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);
  
  // Atmega's Secret Voltmeter setup:
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

  delay(2); // Wait for vref to settle

  Serial.println("Initializing Pyr0-Piezo Sensor...");

  restoreConfig();

  adjustGain();
}

/*------------------------------------------------*/

void loop() {
  if (mainLoop.hasPassed(LOOP_DUR)) {
    mainLoop.restart();
    // Blink LED's on init
    if (BlinkCount > 0) {
      BlinkState = !BlinkState;
      digitalWrite(ERR_LED, BlinkState);
      digitalWrite(TRG_OUT, BlinkState);
      --BlinkCount;
    }

    // Get Serial Input
    serialInput();

    // Set any new parameters from serial input
    if (serialIncoming) {
      updateParams();
    }

    // Set the amplification gain factor
    adjustGain();

    // Check voltage of first and second stages and compare against thresholds
    readVin();
    VComp = analogRead(VCOMP_SENSE_PIN);
    VFol = analogRead(V_FOLLOW_PIN);

    // Voltage Follower adjustment
    VLast = VOld - Vin;
    if (VLast > Hyst || VLast < -Hyst) {
      adjustFollow();

    // Voltage Comparator adjustment
      adjustComp();
    // Alert the user that auto-calibration is ongoing
      ERR_STATE = 1;
    } else {
      ERR_STATE = 0;
    }

    // Blink LED's on init
    if (BlinkCount > 0) {
      BlinkState = !BlinkState;
      digitalWrite(ERR_LED, BlinkState);
      digitalWrite(TRG_OUT, BlinkState);
      --BlinkCount;
    } else {
      // Check for error state
      checkError();
    }

    // Print state if debug is on
    if (Debug > 0) {
      serialPrintState();
    }
    // Sets trigger output state to false after completing loop
    //digitalWrite(TRG_OUT, HIGH);
    sensorHReading = 0;
  }
}
