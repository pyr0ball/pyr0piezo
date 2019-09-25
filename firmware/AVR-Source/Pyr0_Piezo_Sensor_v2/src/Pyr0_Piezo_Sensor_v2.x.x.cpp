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
To change sensor input pullup vRef low threshold: VFOL [float value]
To change comparator trigger high threshold: VCOMP [float value]


These commands should be wrapped in this format:
<CMD, INT>

Examples:
<GAIN_F, 3> <~ set gain factor to index 3 (6x)
<VFOL, 2350> <~ set the vref floor to 2.35V

*Note for Gain Factor:
The gain STATE is representative of these values:
0 = 3x
1 = 3.5x
2 = 4.33x
3 = 6x
4 = 11x
*/

#include <Arduino.h>

// Headers, variables, and functions
#include "LightChrono.h"
#include "pP_pins.h"
#include "pP_config.h"
#include "pP_volatile.h"
#include "pP_function.h"
#include "pP_serial.h"

// i2c input toggle. Uncomment to enable
//#define I2C_INPUT true

void setup()
{
  pinMode(TRG_OUT, OUTPUT); // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(Z_TRG, INPUT_PULLUP); // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VCOMP_SENSE_PIN, INPUT);
  pinMode(GADJ_R0, INPUT); // declare input to set high impedance
  pinMode(GADJ_R1, INPUT); // declare input to set high impedance
  pinMode(GADJ_R2, INPUT); // declare input to set high impedance
  pinMode(GADJ_R3, INPUT); // declare input to set high impedance
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);

  Serial.println("Initializing Pyr0-Piezo Sensor...");

  restoreConfig();

  adjustGain();
}

void loop()
{
  if (mainLoop.hasPassed(LOOP_DUR))
  {
    mainLoop.restart();

    // Get Serial Input
    serialInput();

    // Set any new parameters from serial input
    if (serialIncoming)
    {
      updateParams();
    }

    // Check voltage of first and second stages and compare against thresholds
    readVin();
    VComp = analogRead(VCOMP_SENSE_PIN);
    VFol = analogRead(V_FOLLOW_PIN);

    VLast = VOld - Vin;
    if (VLast > Hyst || VLast < -Hyst)
    {
      // Voltage Follower adjustment
      adjustFollow();
      // Voltage Comparator adjustment
      adjustComp();
      // Alert the user that auto-calibration is ongoing
      ERR_STATE = 1;
    }
    else
    {
      ERR_STATE = 0;
    }

    // Blink LED's on init
    if (BlinkCount > 0)
    {
      BlinkState = !BlinkState;
      digitalWrite(ERR_LED, BlinkState);
      digitalWrite(TRG_OUT, BlinkState);
      --BlinkCount;
    }
    else
    // Check for error state
    {
      checkError();
    }

    // Print state if debug is on
    if (Debug > 0)
    {
      serialPrintState();
    }

    // Sets trigger output state to false after completing loop
    //digitalWrite(TRG_OUT, HIGH);
    sensorHReading = 0;
  }
}
