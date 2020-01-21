#include "pP_config.h"
#include <EEPROM.h>

int GAIN_FACTOR = GAIN_FACTOR_DEFAULT; // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
int followerThrs = FOLLOWER_THRESHOLD_DEFAULT;
int compThrs = COMP_THRESHOLD_DEFAULT;
int LOOP_DUR = LOOP_DUR_DEFAULT; // duration of time between ADC checks and other loop functions
int TRG_DUR = TRG_DUR_DEFAULT;   // duration of the Z-axis pulse sent, in ms
int Hyst = HYST_DEFAULT;         // Hysteresis value for ADC measurements
int LOGIC = LOGIC_DEFAULT;      // Trigger output logic (active low or active high)
int PZDET = PZDET_DEFAULT;      // Enable/disable piezo connection detection
int Debug = 0;
long voltMeterConstant = VM_CONST_DEFAULT;
uint8_t pP_i2c_address = 0xa0;
