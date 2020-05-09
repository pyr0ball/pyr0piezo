/*
  pyr0-piezo functions library
  Created by Alan "pyr0ball" Weinstock 6/26/2019
*/

//#pragma once
//#include "pP_function.h"

#ifndef PP_FUNCTION_H
#define PP_FUNCTION_H

#include "stdint.h"

void digitalWriteFast(uint8_t pin, uint8_t x);
int analogReadFast(uint8_t ADCpin);
void doubleFlash();
void pulse();
long readVcc();
void readVin();
void adjustFollow();
void adjustComp();
void adjustVcc();
void calibrateAlert();
void adjustGain();
void pzConCheck();

#endif // PP_FUNCTION_H