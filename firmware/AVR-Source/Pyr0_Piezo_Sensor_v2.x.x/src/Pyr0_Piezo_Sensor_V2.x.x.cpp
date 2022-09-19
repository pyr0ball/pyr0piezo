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
 * PB6 PCINT6 (Gain Adjustment Resistor 0 'D20')
 * PB7 PCINT7 (Gain Adjustment Resistor 1 'D21')
 * PD5 T1 (Gain Adjustment Resistor 2 'D5')
 * PD6 PCINT22 (Gain Adjustment Resistor 3 'D6')
 * PB0 PCINT0 (VCC Adjustment Resistors 'D8')
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
To change the output logic: LOGIC [0|1] (0 for active low, 1 for active high)
To enable piezo plugged detection: PZDET [0|1] (0 for disabled, 1 for enabled)
To set the sensor's output voltage: VCCSW [0|1] (0 for 3.3v, 1 for 5v)
To change ADC hysteresis value: HYST [integer in millivolts]
To change sensor input pullup vRef low threshold: VFOL [integer in millivolts]
To change comparator trigger high threshold: VCOMP [integer in millivolts]
To change the duration between ADC measurements: LOOP_D [integer in milliseconds]
To update the internal vRef constant value **(see notes below): CONST [long value]

You can also enable or disable DEBUG output with: DEBUG [0|1]

You can query the current configuration with: CONFIG
You can query the current state (including ADC measurements) with: STATE

To set all settings to defaults, use: ERASE

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

// i2c input toggle. Uncomment to enable
#define I2C_INPUT

// Headers, variables, and functions
#include "LightChrono.h"
#include "pP_function.h"
#include "pP_i2c.hpp"
#include "pP_pins.h"
#include "pP_serial.h"
#include "pP_volatile.h"
#include <Arduino.h>

void setup() {
  // Setup PWM on voltage follower (PD3)
  TCCR2A = (1 << COM2B1) | (0 << COM2B0) | (0 << WGM21) | (1 << WGM20);
  TCCR2B = (0 << WGM22) | (0 << CS22) | (0 << CS21) | (1 << CS20);
  DDRD |= (1 << DDD3);

  // Setup PWM on comparator (PB1)
  TCCR1A = (1 << COM1A1) | (0 << COM1A0) | (1 << WGM11) | (1 << WGM10);
  TCCR1B = (0 << WGM13) | (0 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
  DDRB |= (1 << DDB1);

  pinMode(TRG_OUT, OUTPUT); // declare the Trigger as as OUTPUT
  pinMode(ERR_LED, OUTPUT);
  pinMode(PZDET_PIN, INPUT_PULLUP);
  pinMode(Z_TRG, INPUT_PULLUP); // declare z-sense input with pullup
  pinMode(V_FOLLOW_PIN, INPUT);
  pinMode(VCOMP_SENSE_PIN, INPUT);
  pinMode(GADJ_R0, INPUT); // declare input to set high impedance
  pinMode(GADJ_R1, INPUT); // declare input to set high impedance
  pinMode(GADJ_R2, INPUT); // declare input to set high impedance
  pinMode(GADJ_R3, INPUT); // declare input to set high impedance

  attachInterrupt(digitalPinToInterrupt(Z_TRG), pulse, FALLING);

  Serial.begin(9600);
  Serial.print("Pyr0-Piezo Z-Probe Firmware ");
  Serial.println(PP_VERSION);
  Serial.println("Info: https://docs.pyroballpcbs.com");
  Serial.println("Source: https://github.com/pyr0ball/pyr0piezo");
  Serial.println("Initializing Pyr0-Piezo Sensor...");

  i2cInit();

  restoreConfig();

  adjustGain();

  adjustVcc();

  digitalWriteFast(TRG_OUT, !LOGIC);
}

/*------------------------------------------------*/

void loop() {
  if (mainLoop.hasPassed(LOOP_DUR)) {
    mainLoop.restart();

    // Get Serial Input
    serialInput();

    // Set any new parameters from serial input
    if (serialIncoming) {
      updateParams();
    }

    // Set the amplification gain factor
    adjustGain();

    // Set the VCC input switch
    adjustVcc();

    // Check voltage of first and second stages and compare against thresholds
    readVin();
    VComp = analogReadFast(VCOMP_SENSE_PIN);
    VFol = analogReadFast(V_FOLLOW_PIN);

    VLast = VOld - Vin;
    if (VLast > Hyst || VLast < -Hyst) {
      // Voltage Follower adjustment
      adjustFollow();
      // Voltage Comparator adjustment
      adjustComp();
      // Alert the user that auto-calibration is ongoing
      ERR_STATE = 1;
    } else {
      ERR_STATE = 0;
    }

    // Check that the piezo disk is properly connected
    pzConCheck();

    // Print state if debug is on
    if (Debug > 0) {
      serialPrintState();
    }
  }

  // Blink LED
  if (blinkLoop.hasPassed(BLINK_DURATION) && BlinkCount > 0) {
    blinkLoop.restart();
    BlinkCount--;
    digitalWriteFast(ERR_LED, BlinkCount % 2);
  }

  if (lastTriggerTimestamp > 0 && millis() - lastTriggerTimestamp > TRG_DUR) {
    digitalWriteFast(TRG_OUT, !LOGIC);
    lastTriggerTimestamp = 0;
  }
}
