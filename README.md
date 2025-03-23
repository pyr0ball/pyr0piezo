For more extensive documentation, refer to the [Docs Site](https://docs.pyroballpcbs.com) (source included in this repository)

Presenting...

---

# The Pyr0-Piezo Z-Axis Sensor

## What is this thing?

TL;DR: It's a litte circuit that turns your actual nozzle into a z-probe.

This is a Z-Probe Sensor for 3D Printers that utilizes a piezoelectric element (commonly a 20-27mm piezo disk used in musical pickups) to detect when the nozzle of the printer touches the bed. There have been other Z-Probes in the past that do this, but this is the first to incorporate an onboard microcontroller that automatically calibrates and balances the circuit. Previous piezo Z-Probes all required manual tuning to some extent, usually with tiny hard-to-use trimpots. Due to the mechanical and electrical characteristics of this type of sensor, it has several advantages over other Z-Probes:

- There are no offsets to configure or adjust
- It can be configured to emulate either a standard endstop, or any active-high Z-Probe
- It's not dependent on a specific type of bed or hotend
- The builtin signal filtering reduces the amount of false driggers dramatically compared to other piezo sensors.
- It's FAST. Delay between the piezo being actuated and the z-trigger signal being sent to the printer's controller is ***just 26 nanoseconds***

### Is there a catch?

Well, yeah a couple. The sensor requires introducing a small amount of physical instability into the construction of the 3D printer. Something needs to move, even a small fraction of a millimeter. This can be something like the bed being pushed down slightly on it's springs, or adding a hinge and a tensioning spring to the print head. There's innumerable ways to make this work, and I've been working hard with my beta testers to find the best possible method combining as much stability as possble with the highest sensitivity. The other catch is that any oozing filament will skew the leveling results, so you have to decide to either assume there will always be ooze and use a small z-offset, or add a nozzle wipe/clean manuver to your startup gcode before bed leveling.

### How do I use it?

I designed the sensor to connect to a 3D Printer's controller like any other endstop or Z-Probe. The sensor also includes an i2c interface that will allow the 3D Printer's controller to change parameters on the fly depending on the use-case.

The piezo element is mounted somewhere on the 3D Printer in such a way that it undergoes mechanical stress when the nozzle touches the bed. So far there have been three distinct mounting schemes that appear to work well:

- On the extruder assembly
- Under the Print Bed
- In the case of CoreXY or other Gantry Kinematic systems, on either end of the gantry

#### Features:

- Self-calibrating (no more fiddling with tiny potentiometers!)
- Ultra-precise z-height measurements
- Zero offset (The nozzle itself is the sensor!)
- Compatible with ALL surface types
- No plugging in removable sensors for leveling
- Tunable over UART / I2C

#### FFC Cable Chain Extra Features:

- FFC Cable chain for a clean connection between print head and controller
- Onboard switchable DC buck converter for 12v or 5v fan operation
- LED Feedback on all PWM components
 Standalone version available for drop-in installation

Credit must be given to precisionpiezo.co.uk for getting me started on this project and giving me a place to start. I did build a version of the FFC cable chain based on their electrical designs but found the calibration of the circuit to be very fiddly, as the range of value on the potentiometers that was acceptable was very narrow.

I've since started from scratch using my own BOM and designs, while including an onboard microcontroller to handle auto-calibration. Given that every 3D printer is different, I wanted this sensor to be as easy and stable as other sensors available on the market, but with the increased performance of a piezo sensor.

Join our discord for support and to join in realtime discussion for the project:

![Discord Banner 1](https://discordapp.com/api/guilds/544587989536473099/widget.png?style=banner1)

---

## Contribution:

Please submit all Pull Requests through the **develop** branch!

The support site for this project uses a script to automatically parse git commits for changelog data. There are three categories of commits that should show up in the case log:

- Bugfix `#bugfix`: commits related to fixing something that's broken
- Added Features `#featureadd`: Commits related to adding new functions and features
- Documentation `#docs`: Any commits that deal specifically with the [documentation site](https://docs.pyroballpcbs.com) or comments/instructions contained within the source itself but do not contribute any functional changes

Simply by adding the corresponding tag to the end of the first line of your commit message, the changelog will automatically compile these for the documentation site. Please be sure to include this in any commit messages for this project.

I would also request that commits remain small, adding each change/fix/feature to their own commits. This will help with future debugging, version tracking, and testing.

---

This is a complete redesign from the ground up, but was inspired by the sensor developed by [PrecisionPiezoUK](https://precisionpiezo.co.uk/resources-osh)

Original FFC Cable Chain designs can be found on my Thingiverse:

- [3D Printed Mount for Prusa/Anet printers](https://www.thingiverse.com/thing:2712439)
- [FFC Cable Chain + Piezo boards](https://www.thingiverse.com/thing:2618717)

 Please refer to LICENSE.md for license information
