Presenting...
--------------------------------

The Pyr0-Piezo Z-Axis Sensor
===================

**What's this for?**
--------------------

This is a Z-axis sensor for 3D printers.

This design incorporates two different enhancements:

1. An Ultra-Sensitive Z-Axis sensor utilizing the mechanical force of the nozzle touching the bed, giving a direct measurement with no offsets or referential assumptions to consider

2. (Optional) An FFC Cable chain [(Flat Flexible Cable)](https://en.wikipedia.org/wiki/Flexible_flat_cable) that can carry the entire print-head's signals and current in a clean and organized way

**Why is this better than X sensor?**

 - Vs. Inductive / capacitative sensors: No Offset, more accurate, doesn't require special surface

 - Vs. Mechanical touch (BL-Touch, servo mounted endstops): No Offset, more accurate

 - Vs. removable pressure sensor: More accurate, no need for attaching a device only for levelling

 - Vs. Optical rangefinder / Time of Flight: Much more accurate, doesnt require specific surface

 - Vs. Mechanical endstops: All the things

 - Vs [PrecisionPiezoUK](https://www.precisionpiezo.co.uk/resources-osh) version: No potentiometer calibration required, better false-signal filtering

 Features:

 - Self-calibrating (no more fiddling with tiny potentiometers!)
 - Ultra-precise z-height measurements
 - Zero offset (The nozzle itself is the sensor!)
 - Compatible with ALL surface types
 - No plugging in removable sensors for leveling
 - Tunable over UART / I2C

 FFC Cable Chain Extra Features:

 - FFC Cable chain for a clean connection between print head and controller
 - Onboard switchable DC buck converter for 12v or 5v fan operation
 - LED Feedback on all PWM components
 Standalone version available for drop-in installation

Credit must be given to precisionpiezo.co.uk for getting me started on this project and giving me a place to start. I did build a version of the FFC cable chain based on their electrical designs but found the calibration of the circuit to be very fiddly, as the range of value on the potentiometers that was acceptable was very narrow.

I've since started from scratch using my own BOM and designs, while including an onboard microcontroller to handle auto-calibration. Given that every 3D printer is different, I wanted this sensor to be as easy and stable as other sensors available on the market, but with the increased performance of a piezo sensor.

Join our discord for support and to join in realtime discussion for the project:

![Discord Banner 1](https://discordapp.com/api/guilds/544587989536473099/widget.png?style=banner1)
---------------------------

## Contribution:

Please submit all Pull Requests through the **develop** branch!

The support site for this project uses a script to automatically parse git commits for changelog data. There are three categories of commits that should show up in the case log:

  - Bugfix `#bugfix`: commits related to fixing something that's broken
  - Added Features `#featureadd`: Commits related to adding new functions and features
  - Changelog `#changelog`: Any other commits that should be on the changelog, but don't fall into the two categories above

Simply by adding the corresponding tag to the end of the first line of your commit message, the changelog will automatically compile these for the documentation site. Please be sure to include this in any commit messages for this project.

I would also request that commits remain small, adding each change/fix/feature to their own commits. This will help with future debugging, version tracking, and testing.

--------------------------

Legacy links:

[Original Mount (Doesn't work as well as I'd like)](https://www.thingiverse.com/thing:2712439)

[Original Mount v1 (Doesnt work at all)](https://www.thingiverse.com/thing:2675788)

[Original Piezo + FFC board](https://www.thingiverse.com/thing:2618717)

This is a complete redesign from the ground up, but was based on principles and methodologies developed by [PrecisionPiezoUK](https://precisionpiezo.co.uk/resources-osh)

Original FFC Cable Chain designs can be found on my Thingiverse:
 - [3D Printed Mount for Prusa/Anet printers](https://www.thingiverse.com/thing:2712439)
 - [FFC Cable Chain + Piezo boards](https://www.thingiverse.com/thing:2618717)


 Please refer to LICENSE.md for license information
