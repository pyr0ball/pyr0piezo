
//============================= Pyro-Piezo Sensor ===========================
#define PYROPIEZO_SUPPORT             // Enable Pyro-Piezo sensor support
#if ENABLED(PYROPIEZO_SUPPORT)
  //#define PYROPIEZO_I2C               // Use I2C instead of serial for communication
  #if ENABLED(PYROPIEZO_I2C) && ENABLED(EXPERIMENTAL_I2CBUS)
    #define PYROPIEZO_I2C_ADDRESS 0x10 // I2C address of the Pyro-Piezo (default: 0x10)
  #endif
  
  // Default values for the Pyro-Piezo parameters
  #define PYROPIEZO_TRIGGER_DURATION 20   // Trigger duration in ms (1-200)
  #define PYROPIEZO_GAIN 2                // Gain factor (0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x)
  #define PYROPIEZO_LOGIC 1              // Logic state (0=active low, 1=active high)
  #define PYROPIEZO_PIEZO_DETECTION 1    // Piezo detection (0=disabled, 1=enabled)
  #define PYROPIEZO_HYSTERESIS 20        // Hysteresis in mV (1-1000)
  #define PYROPIEZO_VREF_FLOOR 1450      // vRef floor in mV (1-5000)
  #define PYROPIEZO_VREF_COMP 2850       // vRef comparator in mV (1-5000)
  #define PYROPIEZO_LOOP_DURATION 50     // Loop duration in ms (1-200)
  #define PYROPIEZO_VREF_CONSTANT 1125300L // vRef constant value
#endif
