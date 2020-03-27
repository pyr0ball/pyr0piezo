#include "pP_config.h"
#include "pP_function.h"
#include <EEPROM.h>

int GAIN_FACTOR = GAIN_FACTOR_DEFAULT;
int followerThrs = FOLLOWER_THRESHOLD_DEFAULT;
int compThrs = COMP_THRESHOLD_DEFAULT;
int LOOP_DUR = LOOP_DUR_DEFAULT;
int TRG_DUR = TRG_DUR_DEFAULT;
int Hyst = HYST_DEFAULT;
int LOGIC = LOGIC_DEFAULT;
int PZDET = PZDET_DEFAULT;
int Debug = 0;
long voltMeterConstant = VM_CONST_DEFAULT;

/*------------------------------------------------*/
void eraseEEPROM() {

  setDefaultConfig();

  EEPROM.put(GAIN_FACTOR_ADDRESS, GAIN_FACTOR);
  EEPROM.put(FOLLOWER_THRESHOLD_ADDRESS, followerThrs);
  EEPROM.put(COMP_THRESHOLD_ADDRESS, compThrs);
  EEPROM.put(LOOP_DUR_ADDRESS, LOOP_DUR);
  EEPROM.put(TRG_DUR_ADDRESS, TRG_DUR);
  EEPROM.put(HYST_ADDRESS, Hyst);
  EEPROM.put(PZDET_ADDRESS, PZDET);
  EEPROM.put(LOGIC_ADDRESS, LOGIC);
  EEPROM.put(VM_CONST_ADDRESS, voltMeterConstant);
}

// Restore config from EEPROM, otherwise erase config and write to EEPROM
void restoreConfig() {
  int temp;

  bool erase = false;

  EEPROM.get(GAIN_FACTOR_ADDRESS, temp);
  if (temp < 0 || temp > 4) {
    erase = true;
  } else {
    GAIN_FACTOR = temp;
  }

  EEPROM.get(FOLLOWER_THRESHOLD_ADDRESS, temp);
  if (temp < 0 || temp > 5000) {
    erase = true;
  } else {
    followerThrs = temp;
  }

  EEPROM.get(COMP_THRESHOLD_ADDRESS, temp);
  if (temp < 0 || temp > 5000) {
    erase = true;
  } else {
    compThrs = temp;
  }

  EEPROM.get(LOOP_DUR_ADDRESS, temp);
  if (temp < 0 && temp > 1000) {
    erase = true;
  } else {
    LOOP_DUR = temp;
  }

  EEPROM.get(TRG_DUR_ADDRESS, temp);
  if (temp < 0 || temp > 1000) {
    erase = true;
  } else {
    TRG_DUR = temp;
  }

  EEPROM.get(HYST_ADDRESS, temp);
  if (temp < 0 || temp > 1000) {
    erase = true;
  } else {
    Hyst = temp;
  }

  EEPROM.get(PZDET_ADDRESS, temp);
  if (temp < 0 || temp > 1) {
    erase = true;
  } else {
    PZDET = temp;
  }

  EEPROM.get(LOGIC_ADDRESS, temp);
  if (temp < 0 || temp > 1) {
    erase = true;
  } else {
    LOGIC = temp;
  }

  long longTemp;
  EEPROM.get(VM_CONST_ADDRESS, longTemp);
  if (longTemp < 1000000L || longTemp > 1200000L) {
    erase = true;
  } else {
    voltMeterConstant = longTemp;
  }

  if (erase) {
    eraseEEPROM();
  }

  adjustFollow();
  adjustComp();
}

void setDefaultConfig() {
  GAIN_FACTOR = GAIN_FACTOR_DEFAULT;
  followerThrs = FOLLOWER_THRESHOLD_DEFAULT;
  compThrs = COMP_THRESHOLD_DEFAULT;
  LOOP_DUR = LOOP_DUR_DEFAULT;
  TRG_DUR = TRG_DUR_DEFAULT;
  Hyst = HYST_DEFAULT;
  PZDET = PZDET_DEFAULT;
  LOGIC = LOGIC_DEFAULT;
  voltMeterConstant = VM_CONST_DEFAULT;
  adjustFollow();
  adjustComp();
}