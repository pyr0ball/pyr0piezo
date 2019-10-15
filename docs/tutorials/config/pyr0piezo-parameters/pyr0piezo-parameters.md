## Pyr0-Piezo circuit explanation

In order to understand what each parameter does, a basic understanding of the circuit and signal involved is required.

To begin, we start with the piezo element itself, which usually consists of a disk of brass doped with a piezoelectric ceramic. When flexed, bent, or pressed, this disk outputs a voltage. The amount of energy produced is small, however, and can be "noisy". Each stage of the circuit addresses a different aspect of amplifying, filtering, and conditioning the signal to reject false positives and noise.

**Stage 1: Amplification**

The piezo's output is tied to a high impedance input of an [Operational Amplifier](https://www.electronics-tutorials.ws/opamp/opamp_1.html). The Amp stage takes the signal from the piezo and increases it's voltage by a factor, anywhere between 3x and 11x depending on the `GAIN_F` setting. This amplified signal is then passed through to the third stage. The wire between Stage 1 and Stage 3 is also attached to the output of Stage 2 through a high-ohmage resistor. Adjusting the `GAIN_F` setting is the coarsest and easiest way to adjust the sensor's sensitivity.

Use cases for adjusting `GAIN_F`:

 - Sensor is not sensitive enough, or is too sensitive
 - Many false triggers during movement

[Image placeholder. Add a visual of before/after amplification stage]

**Stage 2: Noise Filter & Slow-Recovery Sustain (Voltage Follower)**

This stage utilizes a combination of a PWM-based DAC, another Amp channel, and a high-ohmage resistor. The purpose of this stage is to provide a "Voltage Floor" or a "Virtual Ground" to the Low-Side input of Stage 3. This stage is governed by the `VFOL` setting. The essential effect of this stage is that any signal input from the first stage that falls below the `VFOL` threshold will be filtered out and suppressed. This setting can be used to address a noisy input signal from too much EMI, static electricity, or other factors.

Use case for adjusting `VFOL`:

 - Many false triggers when stationary
 - Many false triggers during movement

[Image placeholder. Add a visual of VFol compared to input signal]

**Stage 3: Comparator Trigger**

This stage of the circuit utilized another PWM-based DAC to set a "trigger threshold", which is governed by the `VCOMP` setting. When the input signal from the previous two stages increases beyond the threshold set by `VCOMP`, the sensor's Z-Min signal is tripped and a Z-Min trigger signal is sent to the control board of the 3D Printer. As the Follower's filtering signal is attached to this input through a high-ohmage resistor, the amount of time it takes for this triggering circuit to reset is extended, which further reduces multiple trigger signals from being sent. By default, `VCOMP` is set to 2.85v which should be compatible with even 3.3v systems, but there could be instances where the `VCOMP` setting is higher than what the rest of the circuit can provide, and therefore no triggering would occur, even with valid input.

Use case for adjusting `VCOMP`:

 - No triggering from the sensor at all
 - Low input voltage

[Image placeholder. Add a visual representation of the comparator signals]

## Other Settings

**Trigger Duration**

The Trigger duration, governed by the `TRG_D` setting, determines the length of the pulse sent to the printer's control board. Too short of a signal might not be registered by the controller, but too long can interfere with the ADC calculations of the firmware.

**Loop Duration**

The Loop duration, governed by the `LOOP_D` setting, determines how long the controller waits between ADC readings which are used to automatically balance the circuit. This setting should be kept as short as possible for the best possible results, but too short could cause hitching or overloading of the software.

**Software Hysteresis**

The hysteresis value, governed by the `HYST` setting, is a variable used in the ADC calculation to determine whether to adjust the PWM-DAC duty cycle. The hysteresis value adds or subtracts that value from the ADC comparator calculation. It gives a buffer to the software, preventing it from making adjustments to the DAC outputs below the value defined by `HYST`.

**Volt Meter Multiplier Constant**

The Voltage Multiplier Constant, governed by the `CONST` setting, is a value used in the software to accurately determine the microcontroller's input voltage. However, due to minor differences in each chip, this value may not be completely accurate for each individual board. Usually the amount of difference is so minor as to not make much of a difference, but if desired, the end-user can adjust this constant value by using the following:

`scale_constant = internal1.1Ref * 1023 * 1000`

where

`internal1.1Ref = 1.1 * Vcc1 (per voltmeter) / Vcc2 (per readVcc() function)`
