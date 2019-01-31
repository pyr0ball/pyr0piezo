Presenting...
--------------------------------

The Pyr0-Piezo Z-Axis Sensor
===================

Features:
 - Self-calibrating (no more fiddling with tiny potentiometers!)
 - Ultra-precise z-height measurements
 - Zero offset (The nozzle itself is the sensor!)
 - Compatible with ALL surface types
 - No plugging in removable sensors for leveling
 - FFC Cable chain for a clean connection between print head and controller
 - Onboard switchable DC buck converter for 12v or 5v fan operation
 - LED Feedback on all PWM components
 - Standalone version available for drop-in installation

Credit must be given to precisionpiezo.co.uk for getting me started on this project and giving me a place to start. I did build a version of the FFC cable chain based on their electrical designs but found the calibration of the circuit to be very fiddly, as the range of value on the potentiometers that was acceptable was very narrow.

I've since started from scratch using my own BOM and designs, while including an onboard microcontroller to handle auto-calibration. Given that every 3D printer is different, I wanted this sensor to be as easy and stable as other sensors available on the market, but with the increased performance of a piezo sensor.

Legacy links:

[Original Mount (Doesn't work as well as I'd like)](https://www.thingiverse.com/thing:2712439)

[Original Mount v1 (Doesnt work at all)](https://www.thingiverse.com/thing:2675788)

[Original Piezo + FFC board](https://www.thingiverse.com/thing:2618717)

This is a complete redesign from the ground up, but was based on principles and methodologies developed by [PrecisionPiezoUK](https://precisionpiezo.co.uk/resources-osh)

Original FFC Cable Chain designs can be found on my Thingiverse:
 - [3D Printed Mount for Prusa/Anet printers](https://www.thingiverse.com/thing:2712439)
 - [FFC Cable Chain + Piezo boards](https://www.thingiverse.com/thing:2618717)
