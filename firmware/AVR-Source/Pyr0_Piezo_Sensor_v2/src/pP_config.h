#ifndef PP_CONFIG_H
#define PP_CONFIG_H

// Configurable settings:

#define GAIN_FACTOR_DEFAULT 2
#define GAIN_FACTOR_ADDRESS 0
#if !(defined(GAIN_FACTOR))
extern int GAIN_FACTOR; // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#endif

#define FOLLOWER_THRESHOLD_DEFAULT 1450
#define FOLLOWER_THRESHOLD_ADDRESS 4
#if !(defined(followerThrs))
extern int followerThrs;
#endif

#define COMP_THRESHOLD_DEFAULT 2850
#define COMP_THRESHOLD_ADDRESS 8
#if !(defined(compThrs))
extern int compThrs;
#endif

#ifndef InitCount
#define InitCount 6 // Number of times to blink the LED on start
#endif

#define LOOP_DUR_DEFAULT 50
#define LOOP_DUR_ADDRESS 12
#if !(defined(LOOP_DUR))
extern int LOOP_DUR; // duration of time between ADC checks and other loop functions
#endif

#define TRG_DUR_DEFAULT 20
#define TRG_DUR_ADDRESS 16
#if !(defined(TRG_DUR))
extern int TRG_DUR; // duration of the Z-axis pulse sent, in ms
#endif

#define HYST_DEFAULT 20
#define HYST_ADDRESS 20
#if !(defined(Hyst))
extern int Hyst; // Hysteresis value for ADC measurements
#endif

#if !(defined(Debug))
extern int Debug;
#endif

#if !(defined(voldMeterConstant))
extern long voltMeterConstant; // For fine tuning input voltage sense
#endif

// #ifdef I2C_INPUT
// #if !(defined(pP_i2c_address))
// extern byte pP_i2c_address = 0xa0; // I2C Bus Address
// #endif
// #endif

void restoreConfig();
void resetConfig();

#endif