#pragma once

// PyroPiezo module for Marlin

#include "../inc/MarlinConfig.h"

// I2C Command Codes
#define PYROPIEZO_CMD_GAIN_F  0x00
#define PYROPIEZO_CMD_VFOL    0x01
#define PYROPIEZO_CMD_VCOMP   0x02
#define PYROPIEZO_CMD_LOOP_D  0x03
#define PYROPIEZO_CMD_TRG_D   0x04
#define PYROPIEZO_CMD_HYST    0x05
#define PYROPIEZO_CMD_LOGIC   0x06
#define PYROPIEZO_CMD_PZDET   0x07
#define PYROPIEZO_CMD_CONST   0x08
#define PYROPIEZO_CMD_CONFIG  0x09
#define PYROPIEZO_CMD_ERASE   0x0a
#define PYROPIEZO_CMD_STATE   0x0b
#define PYROPIEZO_CMD_VCCSW   0x0c

#ifndef PYROPIEZO_I2C_ADDRESS
  #define PYROPIEZO_I2C_ADDRESS 0x10
#endif

// Gain factor indices
enum PyroPiezoGainFactor : uint8_t {
  PYRO_GAIN_3X = 0,     // 3x gain
  PYRO_GAIN_3_5X = 1,   // 3.5x gain
  PYRO_GAIN_4_33X = 2,  // 4.33x gain
  PYRO_GAIN_6X = 3,     // 6x gain
  PYRO_GAIN_11X = 4     // 11x gain
};

// Configuration data structure
typedef struct {
  uint16_t gain_factor;     // Gain factor (0-4)
  uint16_t vref_floor;      // Voltage follower threshold (mV)
  uint16_t vref_comp;       // Comparator threshold (mV)
  uint16_t loop_duration;   // Loop duration (ms)
  uint16_t trigger_duration; // Trigger duration (ms)
  uint16_t hysteresis;      // Hysteresis (mV)
  uint16_t logic_state;     // Logic state (0=active low, 1=active high)
  uint16_t piezo_detection; // Piezo detection (0=disabled, 1=enabled)
  uint16_t vcc_switch;      // VCC switch (0=3.3V, 1=5V)
  uint32_t vref_constant;   // VRef constant
  char version[16];         // Firmware version
} pyropiezo_config_t;

// State data structure
typedef struct {
  uint16_t vin;         // Input voltage (mV)
  uint16_t vcomp;       // Comparator voltage (mV)
  uint16_t vfol;        // Follower voltage (mV)
  uint16_t err_state;   // Error state
  uint16_t pz_state;    // Piezo connected state
} pyropiezo_state_t;

class PyroPiezo {
private:
  static bool initialized;
  
  // Current settings
  static pyropiezo_config_t config;
  static pyropiezo_state_t state;
  
  // I2C communication methods
  static void send_command(uint8_t cmd);
  static void send_command(uint8_t cmd, uint16_t value);
  static void send_command(uint8_t cmd, uint32_t value);
  static uint16_t read_uint16();
  static uint32_t read_uint32();
  
public:
  static void init();
  static bool update_settings();
  static bool read_settings();
  static bool read_state();
  
  // Setters
  static void set_trigger_duration(uint16_t duration);
  static void set_gain_factor(uint8_t gain);
  static void set_logic_state(bool state);
  static void set_piezo_detection(bool enabled);
  static void set_hysteresis(uint16_t hyst);
  static void set_vref_floor(uint16_t floor);
  static void set_vref_comp(uint16_t comp);
  static void set_loop_duration(uint16_t loop);
  static void set_vref_constant(uint32_t constant);
  static void reset_to_defaults();
  
  // Getters
  static uint16_t get_trigger_duration() { return config.trigger_duration; }
  static uint8_t get_gain_factor() { return (uint8_t)config.gain_factor; }
  static bool get_logic_state() { return config.logic_state != 0; }
  static bool get_piezo_detection() { return config.piezo_detection != 0; }
  static uint16_t get_hysteresis() { return config.hysteresis; }
  static uint16_t get_vref_floor() { return config.vref_floor; }
  static uint16_t get_vref_comp() { return config.vref_comp; }
  static uint16_t get_loop_duration() { return config.loop_duration; }
  static uint32_t get_vref_constant() { return config.vref_constant; }
  
  // State getters
  static uint16_t get_input_voltage() { return state.vin; }
  static uint16_t get_comp_voltage() { return state.vcomp; }
  static uint16_t get_follower_voltage() { return state.vfol; }
  static uint16_t get_error_state() { return state.err_state; }
  static uint16_t get_piezo_state() { return state.pz_state; }
};

extern PyroPiezo pyropiezo;
