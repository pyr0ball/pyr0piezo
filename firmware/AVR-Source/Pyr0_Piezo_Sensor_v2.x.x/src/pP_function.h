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
void readVin();
void adjustFollow();
void adjustComp();
void adjustVcc();
void calibrateAlert();
void adjustGain();
//void checkError () {
//  if (ERR_STATE == 1) {
//    digitalWriteFast(ERR_LED, BlinkState);
//    BlinkState = !BlinkState;
//  }
//  else if (ERR_STATE == 0) {
//    BlinkState = LOW;
//    digitalWriteFast(ERR_LED, BlinkState);
//  }
//}
void pzConCheck();

#endif //PP_FUNCTION_H