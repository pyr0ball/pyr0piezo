Monoprice Delta Mini Installation
-------------------------------

#### EasyPiezi Wiring:
All of the MPDM's under-bed switch connectors are tied to the same input pin, so only one board connection is required. However, in order to power the EasyPiezi, two solder bridges must be added (a 0Ω resistor also works).

The first must be added to SJ1 on the bottom of the EasyPiezi:

![](assets/images/mp-delta-mini-ub/5.jpg)

The next must be added to R12 on the MPDM's mainboard, adjacent to the connector used by the EasyPiezi:

![](assets/images/mp-delta-mini-ub/4.jpg)

#### Wiring the piezo disks
Since the EasyPiezi uses a single two-pin input, we need to wire up three piezo disks in parallel (all positive and negative connections are joined together)

![](assets/images/mp-delta-mini-ub/1.jpg)
