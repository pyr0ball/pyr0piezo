#if ENABLED(PYR0PIEZO_SUPPORT)
  #include "../../feature/pyr0piezo.h"
  
  // Helper function to display gain factor as text
  static PGM_P pyr0piezo_gain_factor_str() {
    switch(pyr0piezo.get_gain_factor()) {
      default:
      case PYR0_GAIN_3X:    return PSTR("3.0x");
      case PYR0_GAIN_3_5X:  return PSTR("3.5x");
      case PYR0_GAIN_4_33X: return PSTR("4.33x");
      case PYR0_GAIN_6X:    return PSTR("6.0x");
      case PYR0_GAIN_11X:   return PSTR("11.0x");
    }
  }
  
  void menu_pyr0piezo() {
    const bool piezo_plugged = pyr0piezo.get_piezo_detection();
    
    START_MENU();
    BACK_ITEM(MSG_ADVANCED_SETTINGS);
    
    // Trigger duration
    EDIT_ITEM_FAST(uint16_3, MSG_PYR0_TRIGGER_DURATION, &pyr0piezo.trigger_duration, 1, 200, []{
      pyr0piezo.set_trigger_duration(pyr0piezo.trigger_duration);
    });
    
    // Gain factor
    {
      uint8_t gain_val = pyr0piezo.get_gain_factor();
      EDIT_ITEM_FAST(uint8, MSG_PYR0_GAIN, &gain_val, 0, 4, []{
        pyr0piezo.set_gain_factor(gain_val);
      });
    }
    
    // Logic state
    EDIT_ITEM(bool, MSG_PYR0_LOGIC_STATE, &pyr0piezo.logic_state, []{
      pyr0piezo.set_logic_state(pyr0piezo.logic_state);
    });
    
    // Piezo detection
    EDIT_ITEM(bool, MSG_PYR0_PIEZO_DETECTION, &pyr0piezo.piezo_detection, []{
      pyr0piezo.set_piezo_detection(pyr0piezo.piezo_detection);
    });
    
    // Hysteresis
    EDIT_ITEM_FAST(uint16_4, MSG_PYR0_HYSTERESIS, &pyr0piezo.hysteresis, 1, 1000, []{
      pyr0piezo.set_hysteresis(pyr0piezo.hysteresis);
    });
    
    // VREF floor
    EDIT_ITEM_FAST(uint16_4, MSG_PYR0_VREF_FLOOR, &pyr0piezo.vref_floor, 1, 5000, []{
      pyr0piezo.set_vref_floor(pyr0piezo.vref_floor);
    });
    
    // VREF comparator
    EDIT_ITEM_FAST(uint16_4, MSG_PYR0_VREF_COMP, &pyr0piezo.vref_comp, 1, 5000, []{
      pyr0piezo.set_vref_comp(pyr0piezo.vref_comp);
    });
    
    // Loop duration
    EDIT_ITEM_FAST(uint16_3, MSG_PYR0_LOOP_DURATION, &pyr0piezo.loop_duration, 1, 200, []{
      pyr0piezo.set_loop_duration(pyr0piezo.loop_duration);
    });
    
    // Reset to defaults
    ACTION_ITEM(MSG_PYR0_RESET_DEFAULTS, []{
      pyr0piezo.reset_to_defaults();
    });
    
    END_MENU();
  }
#endif // PYR0PIEZO_SUPPORT

// Add to menu_advanced() function
#if ENABLED(PYR0PIEZO_SUPPORT)
  SUBMENU(MSG_PYR0PIEZO_SETTINGS, menu_pyr0piezo);
#endif
