#ifndef PP_CONFIG_H
#define PP_CONFIG_H

// Configurable settings:

#define GAIN_FACTOR_DEFAULT 2 // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#define GAIN_FACTOR_ADDRESS 0
#if !(defined(GAIN_FACTOR))
 extern int GAIN_FACTOR; 
#endif

#define FOLLOWER_THRESHOLD_DEFAULT 1450 // Voltage follower default voltage in mV
#define FOLLOWER_THRESHOLD_ADDRESS 4
#if !(defined(followerThrs))
  extern int followerThrs;
#endif

#define COMP_THRESHOLD_DEFAULT 2850 // Comparatore Vref default voltage in mV
#define COMP_THRESHOLD_ADDRESS 8
#if !(defined(compThrs))
  extern int compThrs;
#endif

#ifndef InitCount
  #define InitCount 6 // Number of times to blink the LED on start
#endif

#define LOOP_DUR_DEFAULT 50 // duration of time between ADC checks and other loop functions
#define LOOP_DUR_ADDRESS 12
#if !(defined(LOOP_DUR))
  extern int LOOP_DUR; 
#endif

#define TRG_DUR_DEFAULT 20 // duration of the Z-axis pulse sent, in ms
#define TRG_DUR_ADDRESS 16
#if !(defined(TRG_DUR))
  extern int TRG_DUR; 
#endif

#define HYST_DEFAULT 20
#define HYST_ADDRESS 20
#if !(defined(Hyst))
  extern int Hyst; // Hysteresis value for ADC measurements
#endif

#if !(defined(Debug))
 extern int Debug;
#endif

#define VM_CONST_ADDRESS 28
#define VM_CONST_DEFAULT 1125300L
#if !(defined(voltMeterConstant))
  extern long voltMeterConstant; // For fine tuning input voltage sense
#endif

#ifdef I2C_INPUT
  #define I2C_SLAVE_ADDRESS 24
  #if !(defined(pP_i2c_address))
    extern uint8_t pP_i2c_address;     // I2C Bus Address
  #endif
#endif

void restoreConfig();
void resetConfig();

#endif