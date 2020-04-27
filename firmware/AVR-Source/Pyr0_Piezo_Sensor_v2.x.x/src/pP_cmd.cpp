#include "EEPROM.h"
#include "pP_config.h"
#include "pP_function.h"

/*------------------------------------------------*/

void updateGainFactor(int value) {
  if (value >= 0) {
    GAIN_FACTOR = value;
    adjustGain();
    EEPROM.put(GAIN_FACTOR_ADDRESS, GAIN_FACTOR);
  }
}

/*------------------------------------------------*/

void updateVFol(int value) {
  if (value >= 0) {
    followerThrs = value;
    adjustFollow();
    EEPROM.put(FOLLOWER_THRESHOLD_ADDRESS, followerThrs);
  }
}
/*------------------------------------------------*/

void updateVComp(int value) {
  if (value >= 0) {
    compThrs = value;
    adjustComp();
    EEPROM.put(COMP_THRESHOLD_ADDRESS, compThrs);
  }
}

/*------------------------------------------------*/

void updateLoopDuration(int value) {
  if (value >= 0) {
    LOOP_DUR = value;
    EEPROM.put(LOOP_DUR_ADDRESS, LOOP_DUR);
  }
}
/*------------------------------------------------*/

void updateTrigDuration(int value) {
  if (value >= 0) {
    TRG_DUR = value;
    EEPROM.put(TRG_DUR_ADDRESS, TRG_DUR);
  }
}
/*------------------------------------------------*/

void updateHysteresis(int value) {
  if (value >= 0) {
    Hyst = value;
    EEPROM.put(HYST_ADDRESS, Hyst);
  }
}
/*------------------------------------------------*/

void updateLogic(int value) {
  if (value >= 0) {
    LOGIC = value;
    EEPROM.put(LOGIC_ADDRESS, LOGIC);
    pulse();
  }
}
/*------------------------------------------------*/

void updatePzDet(int value) {
  if (value >= 0) {
    PZDET = value;
    EEPROM.put(PZDET_ADDRESS, PZDET);
  }
}
/*------------------------------------------------*/

void updateVccSwitch(int value) {
  if (value >= 0) {
    VCCSW = value;
    EEPROM.put(VCCSW_ADDRESS, VCCSW);
  }
}

void updateConstant(long value) {
  if (value >= 0) {
    voltMeterConstant = value;
    EEPROM.put(VM_CONST_ADDRESS, voltMeterConstant);
  }
}

/*------------------------------------------------*/

void updateDebug(int value) {
  if (value > 0) {
    Debug = 1;
  } else if (value == 0) {
    Debug = 0;
  }
}