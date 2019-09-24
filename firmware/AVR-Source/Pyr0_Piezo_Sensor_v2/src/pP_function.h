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

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = voltMeterConstant / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

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
