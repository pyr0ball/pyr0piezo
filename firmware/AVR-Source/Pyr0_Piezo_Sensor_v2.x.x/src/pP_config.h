#ifndef PP_CONFIG_H
#define PP_CONFIG_H

// Configurable settings:

#define GAIN_FACTOR_DEFAULT 2 // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define GAIN_FACTOR_ADDRESS 0
int GAIN_FACTOR = GAIN_FACTOR_DEFAULT;

#define FOLLOWER_THRESHOLD_DEFAULT 1450 // Voltage follower default voltage in mV
#define FOLLOWER_THRESHOLD_ADDRESS 4
int followerThrs = 1450;

#define COMP_THRESHOLD_DEFAULT 2850 // Comparatore Vref default voltage in mV
#define COMP_THRESHOLD_ADDRESS 8
int compThrs = COMP_THRESHOLD_DEFAULT;

#ifndef InitCount
#define InitCount 6 // Number of times to blink the LED on start
#endif

#define LOOP_DUR_DEFAULT 50 // duration of time between ADC checks and other loop functions
#define LOOP_DUR_ADDRESS 12
int LOOP_DUR = LOOP_DUR_DEFAULT;

#define TRG_DUR_DEFAULT 20 // duration of the Z-axis pulse sent, in ms
#define TRG_DUR_ADDRESS 16
int TRG_DUR = TRG_DUR_DEFAULT;

#define HYST_DEFAULT 20
#define HYST_ADDRESS 20
int Hyst = HYST_DEFAULT; // Hysteresis value for ADC measurements

#define LOGIC_DEFAULT 1
#define LOGIC_ADDRESS 32
int LOGIC = LOGIC_DEFAULT; // Trigger logic scheme, Active LOW is default

#define PZDET_DEFAULT 0
#define PZDET_ADDRESS 26
int PZDET = PZDET_DEFAULT; // Enable or disable piezo connection detection, default is off

int Debug = 0;

#define VM_CONST_ADDRESS 28
#define VM_CONST_DEFAULT 1125300L
long voltMeterConstant = VM_CONST_DEFAULT; // For fine tuning input voltage sense

#ifdef I2C_INPUT
#define I2C_SLAVE_ADDRESS 24
uint8_t pP_i2c_address = 0xa0; // I2C Bus Address
#endif // I2C_INPUT

void eraseEEPROM();
void setDefaultConfig();
void restoreConfig();

#endif // PP_CONFIG_H
