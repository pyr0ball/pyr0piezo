/*
  pyr0-piezo functions library
  Created by Alan "pyr0ball" Weinstock 6/26/2019
*/

//#pragma once
//#include "pP_function.h"

#include "pP_function.h"
#include "Arduino.h"
#include "pP_config.h"
#include "pP_pins.h"
#include "pP_volatile.h"
#include "stdint.h"

void digitalWriteFast(uint8_t pin, uint8_t x) {
  if (pin / 8) { // pin >= 8
    PORTB ^= (-x ^ PORTB) & (1 << (pin % 8));
  } else {
    PORTD ^= (-x ^ PORTD) & (1 << (pin % 8));
  }
}

int analogReadFast(uint8_t ADCpin) {
  byte ADCSRAoriginal = ADCSRA;
  ADCSRA = (ADCSRA & B11111000) | 4;
  int adc = analogRead(ADCpin);
  ADCSRA = ADCSRAoriginal;
  return adc;
}

/*------------------------------------------------*/

void doubleFlash() {
  BlinkCount = 4;
}

/*------------------------------------------------*/

void pulse() {
  digitalWriteFast(TRG_OUT, LOGIC);
  sensorHReading = 1;
  delay(TRG_DUR);
  digitalWriteFast(TRG_OUT, !LOGIC);
  Serial.println("Trig'd!");
  doubleFlash();
}

/*------------------------------------------------*/

long readVcc() {
  // Read 1.1V reference against AVcc

  // Atmega's Secret Voltmeter setup:
  // set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#else
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif

  delay(2); // Wait for vref to settle

  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA, ADSC))
    ; // measuring

  uint8_t low = ADCL;  // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high << 8) | low;

  result = voltMeterConstant / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result;                       // Vcc in millivolts
}

/*-------------------------------------------------
The above function assumes an "ideal" multiplier constant.
Each Atmega chip is slightly different, so it won't be completely accurate
without tuning. Most of the time this won't be necessary, so don't mess
with this if you don't know what you're doing!
The reading can be fine-tuned by using a multimeter, and this equation:

scale_constant = internal1.1Ref * 1023 * 1000

where

internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)

If the scale_constant calculated is different from the default 1125300,
update the voltMeterConstant variable in pP_config.h with the correct value
--------------------------------------------------*/

void readVin() {
  VOld = Vin;
  Vin = readVcc();
  followerLong = followerThrs * 1023L;
  compLong = compThrs * 1023L;
  followerInt = (long long)followerLong / Vin;
  compInt = (long long)compLong / Vin;
  followerInt = (int)followerInt;
  compInt = (int)compInt;
}

/*------------------------------------------------*/

void adjustFollow() {
  /* Compares diffs of threshold vs read value
   if positive, adjusts the follower to within
   the range set above*/
  followerLong = followerThrs * 1023L;
  followerInt = (long long)followerLong / Vin;
  followerInt = (int)followerInt;
  ADJ_FOLLOW = (followerInt / 4);

  // Analog output (PWM) of duty cycle
  OCR2B = ADJ_FOLLOW;
}

/*------------------------------------------------*/

void adjustComp() {
  compLong = compThrs * 1023L;
  compInt = (long long)compLong / Vin;
  compInt = (int)compInt;
  OCR1A = compInt;
}

/*------------------------------------------------*/

void calibrateAlert() {
  VLast = VOld - Vin;
  if (VLast > Hyst || VLast < -Hyst) {
    ERR_STATE = 1;
  }
}

/*------------------------------------------------*/

void adjustGain() {
  switch (GAIN_FACTOR) {
  case 4:
    pinMode(GADJ_R0, OUTPUT);
    digitalWriteFast(GADJ_R0, LOW);
    break;
  case 3:
    pinMode(GADJ_R1, OUTPUT);
    digitalWriteFast(GADJ_R1, LOW);
    pinMode(GADJ_R0, INPUT);
    break;
  case 2:
    pinMode(GADJ_R2, OUTPUT);
    digitalWriteFast(GADJ_R2, LOW);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    break;
  case 1:
    pinMode(GADJ_R3, OUTPUT);
    digitalWriteFast(GADJ_R3, LOW);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    break;
  case 0:
  default:
    pinMode(GADJ_R3, INPUT);
    pinMode(GADJ_R2, INPUT);
    pinMode(GADJ_R1, INPUT);
    pinMode(GADJ_R0, INPUT);
    break;
  }
}

/*------------------------------------------------*/

void adjustSigVol() {
  switch (SIGVOL) {
  case 0:
    pinMode(SIGVOL_PIN, OUTPUT);
    digitalWriteFast(SIGVOL_PIN, LOW);
    break;
  case 1:
  default:
    pinMode(SIGVOL_PIN, INPUT);
    break;
  }
}

/*------------------------------------------------*/

void pzConCheck() {
  PZ_STATE = digitalRead(PZDET_PIN);
  if (PZ_STATE == PZDET) {
    // digitalWriteFast(TRG_OUT, LOGIC);
    ERR_STATE = 1;
  }
}