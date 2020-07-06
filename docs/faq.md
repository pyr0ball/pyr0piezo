**Q:** Pyro-Piezo? EasyPiezi? What's the difference?

**A:** Pyr0-Piezo refers to the circuit that utilizes Piezoelectric Elements to perform auto bed leveling on 3d Printers, while the EasyPiezi is a standalone board dedicated to carrying this circuit. The Pyr0-Piezo circuit can be integrated into other boards and designs, such as the Pyr0-Piezo FFC.

**Q:** What's the difference between the EasyPiezi and the Pyr0-Piezo FFC?

**A:** The Pyr0-Piezo FFC allows the user to replace their existing bulky cable chain with a [Flexible Flat Cables](https://en.wikipedia.org/wiki/Flexible_flat_cable), while also incorporating the Pyr0-Piezo Z-Probe circuit, fan voltage regulation, motor driver protection diodes, and visual feedback LED's

**Q:** Does this sensor support 3.3v logic controllers?

**A:** Yes, if you bought a production (v2.3.0 or later)  board, the logic level of the circuit is configurable to be either 5v or 3.3v. For pre-production (beta) boards, this is possible by following [this guide](tutorials/wiring/easy-piezi/unregulated-operation/)

**Q:** Is it required to use the SingleDisk Piezo Adapters?

**A:** No. the SingleDisk adapter board is simply a carrier for a 12mm piezo disk that has the dual benefit of maximizing signal-to-noise ratio, and providing a consistent mounting scheme. All that's required is a piezelectric element that has positive and negative rails that can be connected to the Pyr0-Piezo circuit.