## Serial Terminal Applications

In order to send commands, you will need an application capable of communicating over serial. Please download and install one of the following:

 - Windows
    - [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
    - [RealTerm](https://sourceforge.net/projects/realterm/)
    - [Termite](https://www.compuphase.com/software_termite.htm)
 - MacOS
 - Linux
    - minicom - `sudo apt install minicom` || `sudo yum install minicom`

## Updatng Pyr0-Piezo Circuit Parameters over Serial

The Pyr0-Piezo onboard microcontroller can be adjusted over UART/TTY/Serial.

To set the below parameters using serial input, use the following:

- To change trigger active duration: `TRG_D [integer for milliseconds]`
- To change gain factor: `GAIN_F [integer for gain state - see note*]`
- To change ADC hysteresis value: `HYST [integer]`
- To change sensor input pullup vRef low threshold: `VFOL [integer in millivolts]`
- To change comparator trigger high threshold: `VCOMP [integer in millivolts]`
- To change the duration between ADC measurements: `LOOP_D [integer in milliseconds]`
- To update the internal vRef constant value **(see notes below): `CONST [long value]`

You can also enable or disable DEBUG output with: `DEBUG [0|1]`

You can query the current configuration with: `CONFIG`
You can query the current state (including ADC measurements) with: `STATE`

To reset all settings to defaults, use: `RESET`

These commands should be wrapped in this format:
`CMD INT`

Examples:

- `GAIN_F 3` <~ set gain factor to index 3 (6x)
- `VFOL 2350` <~ set the vref floor to 2.35V
- `DEBUG 1` <~ Enable debug output
- `STATE` <~ display current sensor status

*Note for Gain Factor:
The gain STATE is representative of these values:

- 0 = 3x
- 1 = 3.5x
- 2 = 4.33x
- 3 = 6x
- 4 = 11x

------------------------

## Calibrating the internal vRef

The adc calculation in this sensor assumes an "ideal" constant value which
is used to convert the internal 1.1v reference.

Each Atmega chip is slightly different, so it won't be completely accurate
without tuning. Most of the time this won't be necessary, so don't mess
with this if you don't know what you're doing!

The reading can be fine-tuned by using a multimeter, and this equation:

`scale_constant = internal1.1Ref * 1023 * 1000`

where

`internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)`

If the scale_constant calculated is different from the default 1125300, update the voltMeterConstant variable in pP_config.h with the correct value or use the `CONST` command

## Configuration in firmware

All default configurations are stored in `pP_config.h`.

`#define GAIN_FACTOR_DEFAULT 2`
 - Gain adjustment factor. 0=3x, 1=3.5x, 2=4.33x, 3=6x, 4=11x


`#define FOLLOWER_THRESHOLD_DEFAULT 1450`
 - Voltage follower default voltage in millivolts (1/1000V)

`#define COMP_THRESHOLD_DEFAULT 2850`
 - Comparatore Vref default voltage in mV

`#define InitCount 6`
 - Number of times to blink the LED on start

`#define LOOP_DUR_DEFAULT 50`
 - Duration of time between ADC checks and other loop functions

`#define TRG_DUR_DEFAULT 20`
 - Duration of the Z-axis pulse sent, in ms

`#define HYST_DEFAULT 20`
 - Software hysteresis for ADC/DAC adjustment, in mV (±20 default)

`extern long voltMeterConstant = 1125300L;`
 - For fine tuning input voltage sense

`extern byte pP_i2c_address = 0xa0;`
 - I2C Bus Slave Address
