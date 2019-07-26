Marlin Configuration
---------------------------
#### Pyr0-Piezo Rev.2.x.x
##### Endstop logic configuration
Comment #define ENDSTOPPULLUP_ZMIN (or other pin if using a different connection)

Set Z_MIN_ENDSTOP_INVERTING to “true”

![](../../assets/images/marlin-config/rev2.x-1.jpg)

##### Enable Interrupts
*This only applies if you are running Marlin v1.1.9 or later*

Uncomment this line for best possible accuracy:


`#define ENDSTOP_INTERRUPTS_FEATURE`

##### Configure Z-Probe Options

 - If using the Z_Min input, uncomment:

 `#define
Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN`

- uncomment:

  `#define FIX_MOUNTED_PROBE`

  `#define PROBING_FANS_OFF`

  `#define DELAY_BEFORE_PROBING 250`

- Set all probe offsets to 0
- Change Z_PROBE_SPEED_SLOW to match FAST

  `#define Z_PROBE_SPEED_SLOW (Z_PROBE_SPEED_FAST)`

- Optional, Uncomment:

  `#define MULTIPLE_PROBING 2`

- Uncomment:

  `#define Z_MIN_PROBE_REPEATABILITY_TEST`

##### Configure Bed Leveling

- Choose your flavor of bed leveling and uncomment the line for it. Theoretically, Unified Bed Levelling is the best choice for this kind of probe

- Optional: Uncomment `#define RESTORE_LEVELING_AFTER_G28` if you don't plan on doing a full G29 after each G28

- Uncomment and set `#define MANUAL_Z_HOME_POS -0.15`

- Uncomment `#define Z_SAFE_HOMING`
  - Default for this setting puts the nozzle in the center of the bed for G28Z, but you can define a corner offset if you wish. This will be performed before any bed levelling procedure
