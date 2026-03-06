#include "../inc/MarlinConfig.h"

#if ENABLED(PYROPIEZO_SUPPORT)

#include "pyropiezo.h"

#include "../gcode/gcode.h"
#include "../module/planner.h"
#include "../module/settings.h"
#include "../HAL/shared/Delay.h"

#if ENABLED(EXPERIMENTAL_I2CBUS)
  #include "../feature/twibus.h"
#endif

// Initialize static variables
bool PyroPiezo::initialized = false;
pyropiezo_config_t PyroPiezo::config = {
  PYRO_GAIN_4_33X,  // gain_factor
  1450,             // vref_floor
  2850,             // vref_comp
  50,               // loop_duration
  20,               // trigger_duration
  20,               // hysteresis
  1,                // logic_state (active high)
  1,                // piezo_detection (enabled)
  1,                // vcc_switch (5V)
  1125300L,         // vref_constant
  ""                // version
};
pyropiezo_state_t PyroPiezo::state = {0};

PyroPiezo pyropiezo;

/**
 * Initialize the PyroPiezo module
 */
void PyroPiezo::init() {
  if (initialized) return;
  
  SERIAL_ECHOLNPGM("Initializing PyroPiezo...");
  
  // Read the current settings from the sensor
  if (read_settings())
    SERIAL_ECHOLNPGM("PyroPiezo initialized successfully");
  else
    SERIAL_ECHOLNPGM("PyroPiezo initialization failed");
    
  initialized = true;
}

/**
 * Send a command with no parameter to the PyroPiezo
 */
void PyroPiezo::send_command(uint8_t cmd) {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    twibus.address(PYROPIEZO_I2C_ADDRESS);
    twibus.addbyte(cmd);
    twibus.send();
  #else
    // Serial implementation as fallback
    switch (cmd) {
      case PYROPIEZO_CMD_CONFIG:
        SERIAL_ECHOLNPGM("CONFIG");
        break;
      case PYROPIEZO_CMD_STATE:
        SERIAL_ECHOLNPGM("STATE");
        break;
      case PYROPIEZO_CMD_ERASE:
        SERIAL_ECHOLNPGM("ERASE");
        break;
    }
  #endif
  
  delay(100); // Give some time for the command to process
}

/**
 * Send a command with a 16-bit value to the PyroPiezo
 */
void PyroPiezo::send_command(uint8_t cmd, uint16_t value) {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    twibus.address(PYROPIEZO_I2C_ADDRESS);
    twibus.addbyte(cmd);
    twibus.addbyte(value >> 8);   // High byte first
    twibus.addbyte(value & 0xFF); // Low byte second
    twibus.send();
  #else
    // Serial implementation as fallback
    char buffer[20];
    const char* cmdString;
    
    switch (cmd) {
      case PYROPIEZO_CMD_GAIN_F: cmdString = "GAIN_F"; break;
      case PYROPIEZO_CMD_VFOL: cmdString = "VFOL"; break;
      case PYROPIEZO_CMD_VCOMP: cmdString = "VCOMP"; break;
      case PYROPIEZO_CMD_LOOP_D: cmdString = "LOOP_D"; break;
      case PYROPIEZO_CMD_TRG_D: cmdString = "TRG_D"; break;
      case PYROPIEZO_CMD_HYST: cmdString = "HYST"; break;
      case PYROPIEZO_CMD_LOGIC: cmdString = "LOGIC"; break;
      case PYROPIEZO_CMD_PZDET: cmdString = "PZDET"; break;
      case PYROPIEZO_CMD_VCCSW: cmdString = "VCCSW"; break;
      default: cmdString = "UNKNOWN"; break;
    }
    
    sprintf_P(buffer, PSTR("%s %d"), cmdString, value);
    SERIAL_ECHOLN(buffer);
  #endif
  
  delay(100); // Give some time for the command to process
}

/**
 * Send a command with a 32-bit value to the PyroPiezo
 */
void PyroPiezo::send_command(uint8_t cmd, uint32_t value) {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    twibus.address(PYROPIEZO_I2C_ADDRESS);
    twibus.addbyte(cmd);
    twibus.addbyte(value >> 24);       // Highest byte
    twibus.addbyte((value >> 16) & 0xFF); // High middle byte
    twibus.addbyte((value >> 8) & 0xFF);  // Low middle byte
    twibus.addbyte(value & 0xFF);       // Lowest byte
    twibus.send();
  #else
    // Serial implementation as fallback
    char buffer[20];
    
    if (cmd == PYROPIEZO_CMD_CONST) {
      sprintf_P(buffer, PSTR("CONST %ld"), value);
      SERIAL_ECHOLN(buffer);
    }
  #endif
  
  delay(100); // Give some time for the command to process
}

/**
 * Read a 16-bit unsigned integer from the I2C bus
 */
uint16_t PyroPiezo::read_uint16() {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    uint8_t high = twibus.readbyte();
    uint8_t low = twibus.readbyte();
    return ((uint16_t)high << 8) | low;
  #else
    return 0; // Cannot read from serial implementation
  #endif
}

/**
 * Read a 32-bit unsigned integer from the I2C bus
 */
uint32_t PyroPiezo::read_uint32() {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    uint8_t byte3 = twibus.readbyte();
    uint8_t byte2 = twibus.readbyte();
    uint8_t byte1 = twibus.readbyte();
    uint8_t byte0 = twibus.readbyte();
    return ((uint32_t)byte3 << 24) | ((uint32_t)byte2 << 16) | ((uint16_t)byte1 << 8) | byte0;
  #else
    return 0; // Cannot read from serial implementation
  #endif
}

/**
 * Read configuration from the PyroPiezo
 */
bool PyroPiezo::read_settings() {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    // Send CONFIG command
    twibus.address(PYROPIEZO_I2C_ADDRESS);
    twibus.addbyte(PYROPIEZO_CMD_CONFIG);
    twibus.send();
    
    // Request data from the device
    if (twibus.request(PYROPIEZO_I2C_ADDRESS, 255)) {
      // Read values in the same order as the firmware sends them
      config.gain_factor = read_uint16();
      config.vref_floor = read_uint16();
      config.vref_comp = read_uint16();
      config.loop_duration = read_uint16();
      config.trigger_duration = read_uint16();
      config.hysteresis = read_uint16();
      config.logic_state = read_uint16();
      config.piezo_detection = read_uint16();
      config.vcc_switch = read_uint16();
      config.vref_constant = read_uint32();
      
      // Read version string - simplified, should be customized for TWI bus
      // In the actual implementation, this would need proper string handling
      for (uint8_t i = 0; i < 15; i++) {
        config.version[i] = twibus.readbyte();
        if (config.version[i] == 0) break;
      }
      config.version[15] = 0; // Ensure null termination
      
      return true;
    }
    return false;
  #else
    // Serial implementation has limited capability for reading back
    // Just use default values
    return true;
  #endif
}

/**
 * Read current state from the PyroPiezo
 */
bool PyroPiezo::read_state() {
  #if ENABLED(EXPERIMENTAL_I2CBUS)
    // Send STATE command
    twibus.address(PYROPIEZO_I2C_ADDRESS);
    twibus.addbyte(PYROPIEZO_CMD_STATE);
    twibus.send();
    
    // Request data from the device
    if (twibus.request(PYROPIEZO_I2C_ADDRESS, 10)) {
      // Read values in the same order as the firmware sends them
      state.vin = read_uint16();
      state.vcomp = read_uint16();
      state.vfol = read_uint16();
      state.err_state = read_uint16();
      state.pz_state = read_uint16();
      
      return true;
    }
    return false;
  #else
    // Serial implementation has limited capability for reading back
    return false;
  #endif
}

/**
 * Update all settings in the config struct to the device
 */
bool PyroPiezo::update_settings() {
  // Update all settings
  send_command(PYROPIEZO_CMD_GAIN_F, config.gain_factor);
  send_command(PYROPIEZO_CMD_VFOL, config.vref_floor);
  send_command(PYROPIEZO_CMD_VCOMP, config.vref_comp);
  send_command(PYROPIEZO_CMD_LOOP_D, config.loop_duration);
  send_command(PYROPIEZO_CMD_TRG_D, config.trigger_duration);
  send_command(PYROPIEZO_CMD_HYST, config.hysteresis);
  send_command(PYROPIEZO_CMD_LOGIC, config.logic_state);
  send_command(PYROPIEZO_CMD_PZDET, config.piezo_detection);
  send_command(PYROPIEZO_CMD_CONST, config.vref_constant);
  
  return true;
}

// Setter methods
void PyroPiezo::set_trigger_duration(uint16_t duration) {
  config.trigger_duration = duration;
  send_command(PYROPIEZO_CMD_TRG_D, duration);
}

void PyroPiezo::set_gain_factor(uint8_t gain) {
  if (gain > PYRO_GAIN_11X) gain = PYRO_GAIN_11X;
  config.gain_factor = gain;
  send_command(PYROPIEZO_CMD_GAIN_F, gain);
}

void PyroPiezo::set_logic_state(bool state) {
  config.logic_state = state ? 1 : 0;
  send_command(PYROPIEZO_CMD_LOGIC, config.logic_state);
}

void PyroPiezo::set_piezo_detection(bool enabled) {
  config.piezo_detection = enabled ? 1 : 0;
  send_command(PYROPIEZO_CMD_PZDET, config.piezo_detection);
}

void PyroPiezo::set_hysteresis(uint16_t hyst) {
  config.hysteresis = hyst;
  send_command(PYROPIEZO_CMD_HYST, hyst);
}

void PyroPiezo::set_vref_floor(uint16_t floor) {
  config.vref_floor = floor;
  send_command(PYROPIEZO_CMD_VFOL, floor);
}

void PyroPiezo::set_vref_comp(uint16_t comp) {
  config.vref_comp = comp;
  send_command(PYROPIEZO_CMD_VCOMP, comp);
}

void PyroPiezo::set_loop_duration(uint16_t loop) {
  config.loop_duration = loop;
  send_command(PYROPIEZO_CMD_LOOP_D, loop);
}

void PyroPiezo::set_vref_constant(uint32_t constant) {
  config.vref_constant = constant;
  send_command(PYROPIEZO_CMD_CONST, constant);
}

void PyroPiezo::reset_to_defaults() {
  send_command(PYROPIEZO_CMD_ERASE);
  
  // Read the updated settings after reset
  read_settings();
}

#endif // PYROPIEZO_SUPPORT
