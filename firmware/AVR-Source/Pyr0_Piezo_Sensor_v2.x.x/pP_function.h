/*
  pyr0-piezo functions library
  Created by Alan "pyr0ball" Weinstock 6/26/2019
*/

void digitalWriteFast(uint8_t pin, uint8_t x) {
  if (pin / 8) { // pin >= 8
    PORTB ^= (-x ^ PORTB) & (1 << (pin % 8));
  }
  else {
    PORTD ^= (-x ^ PORTD) & (1 << (pin % 8));
  }
}

/*------------------------------------------------*/

void pulse() {
  digitalWriteFast(TRG_OUT, LOW);
  sensorHReading = 1;
  #ifdef DEBUG
    Serial.println("Trig!");
  #endif
  delay(TRG_DUR);
  digitalWriteFast(TRG_OUT, HIGH);
}

/*------------------------------------------------*/

long readVcc() {
  // Read 1.1V reference against AVcc
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = voltMeterConstant / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

/* The above function assumes an "ideal" multiplier constant.
Each Atmega chip is slightly different, so it won't be completely accurate
without tuning. Most of the time this won't be necessary, so don't mess
with this if you don't know what you're doing!
The reading can be fine-tuned by using a multimeter, and this equasion:

scale_constant = internal1.1Ref * 1023 * 1000

where

internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)

If the scale_constant calculated is different from the default 1125300,
update the voltMeterConstant variable in pP_config.h with the correct value*/

/*------------------------------------------------*/

 void adjustVin() {
   VOld = Vin;
	 Vin = readVcc(), DEC;
   senseLong = senseThrs * 1024L;
   compLong = compThrs * 1024L;
   senseInt = (long long) senseLong / Vin;
   compInt = (long long) compLong / Vin;
   senseInt = (int) senseInt;
   compInt = (int) compInt;
 }

/*------------------------------------------------*/

 void adjustFollow() {
  /* Compares diffs of threshold vs read value
   if positive, adjusts the follower to within
   the range set above*/
  ADJ_FOLLOW = (senseInt / 4);

  // Analog output (PWM) of duty cycle
  analogWrite(V_FOL_PWM, ADJ_FOLLOW);
}

/*------------------------------------------------*/

void adjustComp() {
  ADJ_COMP = (compInt / 4);

  analogWrite(VCOMP_PWM, ADJ_COMP);
}

/*------------------------------------------------*/

void calibrateAlert() {
  VLast = VOld - Vin;
  if (VLast > Hyst || VLast < -Hyst ) {
    ERR_STATE = 1;
  }
}

/*------------------------------------------------*/

void adjustGain() {

  if (GAIN_FACTOR == 0) {
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
