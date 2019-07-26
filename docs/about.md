### What's this for?

An Ultra-Sensitive Z-probe sensor utilizing the mechanical force of the nozzle touching the bed, giving a direct measurement with no offsets or referential assumptions to consider

Optionally there is a version that incorporates an FFC Cable chain (Flat Flexible Cable) that can carry the entire print-head's signals and current in a clean and organized way

### Why is this better than `<X>` sensor?

- Vs. Inductive / capacitative sensors:

  No Offset, more accurate, doesn't require special surface

- Vs. Mechanical touch (BL-Touch, servo mounted endstops):

  No Offset, more accurate

- Vs. removable pressure sensor:

  More accurate, no need for attaching a device only for levelling

- Vs. Optical rangefinder / Time of Flight:

  Much more accurate, doesnt require specific surface

- Vs. Mechanical endstops:

  All the things

- Vs PrecisionPiezoUK version:

  No potentiometer calibration required, better false-signal filtering

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
