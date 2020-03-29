#define ARDUINO_AVR_ATmega328PB

#include <Arduino.h>
#include <Wire.h>

#include "pP_serial.h"

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing Pyr0-Piezo i2c Bridge...");

  Wire.begin();
}

void loop() {
  serialInput();

  if (serialIncoming) {
    updateParams();
  }
}