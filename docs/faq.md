**Q:** Pyro-Piezo? EasyPiezi? What's the difference?

**A:** Pyr0-Piezo refers to the circuit that utilizes Piezoelectric Elements to perform auto bed leveling on 3d Printers, while the EasyPiezi is a standalone board dedicated to carrying this circuit. The Pyr0-Piezo circuit can be integrated into other boards and designs, such as the Pyr0-Piezo FFC.

**Q:** What's the difference between the EasyPiezi and the Pyr0-Piezo FFC?

**A:** The Pyr0-Piezo FFC allows the user to replace their existing bulky cable chain with a [Flexible Flat Cables](https://en.wikipedia.org/wiki/Flexible_flat_cable), while also incorporating the Pyr0-Piezo Z-Probe circuit, fan voltage regulation, motor driver protection diodes, and visual feedback LED's

**Q:** Does this sensor support 3.3v logic controllers?

**A:** Yes, if you bought a production (v2.3.0 or later)  board, the logic level of the circuit is configurable to be either 5v or 3.3v. For pre-production (beta) boards, this is possible by following [this guide](tutorials/wiring/easy-piezi/unregulated-operation/)

**Q:** Is it required to use the SingleDisk Piezo Adapters?

**A:** No. the SingleDisk adapter board is simply a carrier for a 12mm piezo disk that has the dual benefit of maximizing signal-to-noise ratio, and providing a consistent mounting scheme. All that's required is a piezelectric element that has positive and negative rails that can be connected to the Pyr0-Piezo circuit.

**Q:** Will the sensor work if it experiences constant spring pressure?

**A:** Yes assuming the polarity is correct, you can even pre-tension the piezo and pick up the "relief" motion if you hook it up backwards

**Q:** Why don't you offer drilled piezo disks or use them in your designs?

**A:** You can certainly use drilled disk and make it work with the Pyr0-Piezo, but I don't recommend using drilled discs. You'll get much more consistent results by bending the disk across it's diametric center by allowing one side of the disk to be free moving, and the other statically mounted. The SingleDisk Adapter PCB uses 12mm disks and produces the same signal to noise ratio as a 27mm drilled disk. You can find those [in the shop](https://pyroballpcbs.com/product/pp-singledisk-adapter-pcb/), or you can use the sources in the hardware section of [this repository](https://github.com/pyr0ball/pyr0piezo) to send out to JLC or another fab house if you'd like to build your own.

**Q:** How does heat affect the performance and / or lifespan of the sensors?

**A:** Piezo elements have a particular behavior when it comes to heat, which is that their sensitivity is reduced as more heat is applied. That signal degradation is partially elastic, partially plastic. An analogy that fits is bending a piece of wire. If you deflect a copper wire a small amount, it will return to its original shape, which is an "elastic deformation", but if you bend it further, it will stay in it's deflected state, which is a "plastic deformation". The piezo elements' signal output has a similar effect with heat. After a certain temperature, the signal suppression becomes permanent, but below that threshold, it's elastic. That threshold is different between all elements, so you'll have to test with the ones you've got to find out how much heat they can take.

There's a couple of ways do account for this. The Pyr0-Piezo has the ability to increase it's input signal gain on the fly, so even if the signal from your piezo elements is suppressed, it can be boosted by the sensor to a certain extent. It's also possible to increase the gain beyond the 11x limit of the software, but that would require changing a resistor on the board. If your particular setup requires a higher input gain, and you don't want to do your own modifications, I can rework a unit for you upon request.

Another way to address that issue is to migrate the piezo elements to another location that's still mechanically actuated by the nozzle touching the bed, like under the bed mount screws/springs, or by sandwiching an element between the X/Y gantry carriages of the printer.

**Q:** Why is center of my print bed leveling differently than the edges?

**A:** Likely the print bed is too flexible, and is bowing during probing. You may need to add a rigid frame, or an additional plate to increase the rigidity of the bed. Combining several piezo elements under the bed, and on the print-head can also partially mitigate this issue.

**Q:** Does filament ooze effect my probing?

**A:** It does! Theres a few ways to handle ooze while probing:

- Add a nozzle cleaning routine to your printer's startup behavior (or to the G29/ABL function alone)
- Account for the offset with `Z-OFFSET` in your printer's configuration
- Only probe with the bed at full temperature, and the nozzle below the glass-transition temperature of the filament
