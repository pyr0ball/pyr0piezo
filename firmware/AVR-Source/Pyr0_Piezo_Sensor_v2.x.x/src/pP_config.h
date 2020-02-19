#ifndef PP_CONFIG_H
#define PP_CONFIG_H

// Configurable settings:

#define GAIN_FACTOR_DEFAULT 2 // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define GAIN_FACTOR_ADDRESS 0
extern int GAIN_FACTOR;

#define FOLLOWER_THRESHOLD_DEFAULT 1450 // Voltage follower default voltage in mV
#define FOLLOWER_THRESHOLD_ADDRESS 4
extern int followerThrs;

#define COMP_THRESHOLD_DEFAULT 2850 // Comparatore Vref default voltage in mV
#define COMP_THRESHOLD_ADDRESS 8
extern int compThrs;

#ifndef InitCount
#define InitCount 6 // Number of times to blink the LED on start
#endif

#define LOOP_DUR_DEFAULT 50 // duration of time between ADC checks and other loop functions
#define LOOP_DUR_ADDRESS 12
extern int LOOP_DUR;

#define TRG_DUR_DEFAULT 20 // duration of the Z-axis pulse sent, in ms
#define TRG_DUR_ADDRESS 16
extern int TRG_DUR;

#define HYST_DEFAULT 20
#define HYST_ADDRESS 20
extern int Hyst; // Hysteresis value for ADC measurements

#define LOGIC_DEFAULT 1
#define LOGIC_ADDRESS 32
extern int LOGIC; // Trigger logic scheme, Active LOW is default

#define PZDET_DEFAULT 0
#define PZDET_ADDRESS 26
extern int PZDET; // Enable or disable piezo connection detection, default is off

extern int Debug;

#define VM_CONST_ADDRESS 28
#define VM_CONST_DEFAULT 1125300L
extern long voltMeterConstant; // For fine tuning input voltage sense

#ifdef I2C_INPUT
#define I2C_SLAVE_ADDRESS 24
uint8_t pP_i2c_address = 0xa0; // I2C Bus Address
#endif // I2C_INPUT

void eraseEEPROM();
void setDefaultConfig();
void restoreConfig();

#endif // PP_CONFIG_H
