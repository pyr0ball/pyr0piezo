// Configurable settings:

#if !(defined(GAIN_FACTOR))
  int GAIN_FACTOR = 2;            // Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x
#endif

#ifndef senseThrs
  #define senseThrs 1450
#endif

#ifndef compThrs
  #define compThrs 2850
#endif

#ifndef InitCount
  #define InitCount 6             // Number of times to blink the LED on start
#endif

#if !(defined(LOOP_DUR))
  extern int LOOP_DUR; 
#endif

#if !(defined(TRG_DUR))
  int TRG_DUR = 20;               // duration of the Z-axis pulse sent, in ms
#endif

#if !(defined(Hyst))
  int Hyst = 20;                  // Hysteresis value for ADC measurements
#endif

#if !(defined(voldMeterConstant))
  long voltMeterConstant = 1125300L; // For fine tuning input voltage sense
#endif

#ifdef I2C_INPUT
  #if !(defined(pP_i2c_address))
    byte pP_i2c_address = 0xa0;     // I2C Bus Address
  #endif
#endif

  
