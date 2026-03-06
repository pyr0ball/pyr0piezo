/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2023 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../inc/MarlinConfig.h"

#if ENABLED(PYR0PIEZO_SUPPORT)

#include "../gcode.h"
#include "../../feature/pyr0piezo.h"

/**
 * M952: Control Pyr0-Piezo sensor parameters
 *
 *  With no parameters: Report current settings
 *
 *  D<ms>      - Set trigger duration in milliseconds
 *  G<int>     - Set gain factor (0:3x, 1:3.5x, 2:4.33x, 3:6x, 4:11x)
 *  L<bool>    - Set logic state (0:active low, 1:active high)
 *  P<bool>    - Set piezo detection (0:disabled, 1:enabled)
 *  H<int>     - Set hysteresis in millivolts
 *  F<int>     - Set vRef floor in millivolts
 *  C<int>     - Set vRef comparator in millivolts
 *  I<ms>      - Set loop duration in milliseconds
 *  V<int>     - Set vRef constant value
 *  R          - Reset to default settings
 */
void GcodeSuite::M952() {
  // Initialize the Pyr0-Piezo if not already done
  pyr0piezo.init();
  
  // No parameters? Report settings
  if (!parser.seen("DGLPHFCIVR")) {
    SERIAL_ECHO_START();
    SERIAL_ECHOLNPGM("Pyr0-Piezo Settings:");
    
    SERIAL_ECHOLNPAIR(" Trigger Duration (ms): ", pyr0piezo.get_trigger_duration());
    
    SERIAL_ECHOPGM(" Gain Factor: ");
    switch (pyr0piezo.get_gain_factor()) {
      case PYR0_GAIN_3X:    SERIAL_ECHOLNPGM("3x"); break;
      case PYR0_GAIN_3_5X:  SERIAL_ECHOLNPGM("3.5x"); break;
      case PYR0_GAIN_4_33X: SERIAL_ECHOLNPGM("4.33x"); break;
      case PYR0_GAIN_6X:    SERIAL_ECHOLNPGM("6x"); break;
      case PYR0_GAIN_11X:   SERIAL_ECHOLNPGM("11x"); break;
      default:              SERIAL_ECHOLNPGM("Unknown"); break;
    }
    
    SERIAL_ECHOLNPAIR(" Logic State: ", pyr0piezo.get_logic_state() ? "Active High" : "Active Low");
    SERIAL_ECHOLNPAIR(" Piezo Detection: ", pyr0piezo.get_piezo_detection() ? "Enabled" : "Disabled");
    SERIAL_ECHOLNPAIR(" Hysteresis (mV): ", pyr0piezo.get_hysteresis());
    SERIAL_ECHOLNPAIR(" vRef Floor (mV): ", pyr0piezo.get_vref_floor());
    SERIAL_ECHOLNPAIR(" vRef Comparator (mV): ", pyr0piezo.get_vref_comp());
    SERIAL_ECHOLNPAIR(" Loop Duration (ms): ", pyr0piezo.get_loop_duration());
    SERIAL_ECHOLNPAIR(" vRef Constant: ", pyr0piezo.get_vref_constant());
    
    return;
  }
  
  // Process parameters
  if (parser.seen('D')) pyr0piezo.set_trigger_duration(parser.value_int());
  if (parser.seen('G')) pyr0piezo.set_gain_factor(parser.value_byte());
  if (parser.seen('L')) pyr0piezo.set_logic_state(parser.value_bool());
  if (parser.seen('P')) pyr0piezo.set_piezo_detection(parser.value_bool());
  if (parser.seen('H')) pyr0piezo.set_hysteresis(parser.value_int());
  if (parser.seen('F')) pyr0piezo.set_vref_floor(parser.value_int());
  if (parser.seen('C')) pyr0piezo.set_vref_comp(parser.value_int());
  if (parser.seen('I')) pyr0piezo.set_loop_duration(parser.value_int());
  if (parser.seen('V')) pyr0piezo.set_vref_constant(parser.value_long());
  if (parser.seen('R')) pyr0piezo.reset_to_defaults();
}

#endif // PYR0PIEZO_SUPPORT
