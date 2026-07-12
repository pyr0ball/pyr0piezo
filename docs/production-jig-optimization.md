# Programming Jig Optimization for Production Runs

## Current Jig Design Summary

Your v2.1.2 programmer jig uses:
- **Amphenol 10104320 spring-loaded pogo pin contacts** (3.48x1.1mm)
- **2x parallel ICSP interfaces** (can program 2 boards simultaneously)
- **Individual UART test point contacts** (RXD, TXD, RST, 5V, GND)
- **Micro USB input** with CP2102N USB-to-UART bridge
- **Adjustable mechanical mounting** with M3 screws

## Critical Issues for 400-Board Production Run

### 1. Pogo Pin Wear (HIGHEST PRIORITY)
**Problem:** Standard pogo pins are rated for 1,000 cycles, but quality degrades significantly after 200-300 insertions under production use.

**Impact:** With 400 boards, you're likely to experience contact failures midway through.

**Solutions:**
- **Immediate:** Order spare spring contact assemblies NOW (Amphenol 10104320 or equivalent)
- **Short-term:** Track insertion count and swap contacts every 200 boards
- **Long-term:** Upgrade to Mill-Max 0906 series (10,000+ cycle rating)

**Part recommendations:**
```
Mill-Max 0906-1-15-20-75-14-11-0
- 10,000+ insertion cycles
- 0.9mm diameter receptacle
- 20g actuation force
- $2-3 each (DigiKey: ED90609-ND)
```

### 2. Alignment Repeatability
**Problem:** Manual board positioning leads to:
- Inconsistent contact pressure
- Misaligned pogo pins (causes intermittent connection)
- Slower board insertion/removal

**Solution:** Add precision alignment features

### 3. No Contact Verification
**Problem:** You can't tell if the board is properly seated before flashing starts.

**Impact:** Wasted time flashing boards that aren't making good contact.

**Solution:** Add electrical continuity check or mechanical interlock

## Recommended Improvements (Priority Order)

### Priority 1: Contact Verification Circuit ⚡
Add a simple continuity check before flashing begins.

**Implementation:**
1. Add a pullup resistor (10kΩ) on programmer side connected to a GPIO
2. Connect to one of the ICSP GND pins through the pogo pin
3. Board GND completes the circuit when seated
4. Software checks continuity before attempting flash

**Code integration:**
```python
# In batch-flash.py, add before flash_board():
def verify_contact(self, port):
    """Check if board is properly seated via continuity"""
    # Use DTR/RTS pin as GPIO via serial port
    ser = serial.Serial(port)
    ser.dtr = True
    time.sleep(0.01)

    # Check if signal is pulled low (board connected)
    if ser.cts:  # or ser.dsr depending on wiring
        return True
    return False
```

### Priority 2: Quick-Change Pogo Pin Sockets 🔧
Design pogo pin contacts as replaceable modules.

**Design:**
- Mill PCB with standard 2.54mm header footprint
- Use pogo pin "receptacles" that press-fit or solder to header
- Entire contact array can be swapped in 30 seconds

**BOM Addition:**
```
Pogo Pin Receptacle: Mill-Max 0906-1-15-20-75-14-11-0 (qty: 12)
2.54mm Header: Sullins PREC003SAAN-M71-RC (qty: 2)
Total cost: ~$40 per complete set
Order 2 spare sets for 400-board run
```

### Priority 3: Mechanical Alignment Jig 📐
Create a fixture that ensures perfect board placement every time.

**Design options:**

**Option A: Laser-Cut Acrylic Alignment Plate**
```
Materials:
- 3mm clear acrylic
- 2x M3 alignment pins (10mm shoulder screws)
- 4x M3 compression springs

Features:
- Board drops into precisely cut opening
- Alignment pins engage board mounting holes
- Spring-loaded top plate pushes board onto pogo pins
- Lever actuator for one-hand operation
```

**Option B: 3D Printed Clam-Shell Fixture**
```
Materials:
- 3D printed PETG or ABS
- 2x toggle clamps (small)
- Alignment pins from jig base

Features:
- Hinged top half
- Board slides in, clamp closes
- Positive stop ensures consistent pressure
- Can be operated with one hand
```

### Priority 4: Dual-Station Setup 🔄
Use both ICSP headers to flash 2 boards simultaneously.

**Requirements:**
- Modify batch-flash.py to support dual programming
- Need 2x alignment fixtures (see Priority 3)
- Doubles throughput (200 insertions instead of 400)

**Software modification:**
```python
# Add to batch-flash.py
def flash_dual_boards(self, port):
    """Flash two boards simultaneously via ICSP0 and ICSP2"""
    # Both boards share same UART, so program sequentially
    # but use parallel ICSP verification
    pass
```

### Priority 5: Cycle Counter & Maintenance Alert 📊
Track pogo pin usage and alert when replacement needed.

**Implementation:**
Add to batch-flash.py:
```python
self.jig_insertion_count = 0
self.pogo_pin_warning_threshold = 200

# In flash_board():
self.jig_insertion_count += 1
if self.jig_insertion_count % 50 == 0:
    print(f"⚠ Jig insertions: {self.jig_insertion_count}")
if self.jig_insertion_count >= self.pogo_pin_warning_threshold:
    print(f"🔧 WARNING: Consider replacing pogo pins (>{self.pogo_pin_warning_threshold} cycles)")
```

## Production Workflow Optimization

### Current Estimated Time per Board:
```
Board insertion:          5-10s
Alignment verification:   2-5s
Press ENTER:             1s
Bootloader flash:        15s
Firmware flash:          10s
Verification:            3s
Board removal:           3-5s
-----------------------------------
Total per board:         39-49s
Total for 400 boards:    4.3-5.4 hours
```

### Optimized Time with Improvements:
```
Board insertion (Priority 3):     3s   (fixture alignment)
Contact check (Priority 1):       0.5s (automatic)
Auto-start (no ENTER):            0s   (contact triggers)
Bootloader flash:                 15s
Firmware flash:                   10s
Verification:                     3s
Board removal (fixture):          2s
-----------------------------------
Total per board:                  33.5s
Total for 400 boards:             3.7 hours

With dual station (Priority 4):
Total for 400 boards:             1.85 hours (effective)
```

## Quick Wins You Can Implement Today

### 1. Pre-built Firmware (Saves 2-3s per board)
```bash
cd firmware/AVR-Source/Pyr0_Piezo_Sensor_v2.x.x
pio run -e ATmega328PB
# Use the built .hex file
python batch-flash.py --firmware .pio/build/ATmega328PB/firmware.hex
```

### 2. Keyboard Macro for ENTER (Saves ~1s per board)
On Linux with `xdotool`:
```bash
# When serial device appears, auto-press ENTER after 1 second
while true; do
    if [ -e /dev/ttyUSB0 ]; then
        sleep 1
        xdotool key Return
    fi
    sleep 0.5
done
```

### 3. Audio Feedback Enhancement
Modify batch-flash.py to play distinct sounds:
```python
def beep(self, success=True):
    if success:
        os.system('paplay /usr/share/sounds/freedesktop/stereo/complete.oga &')
    else:
        os.system('paplay /usr/share/sounds/freedesktop/stereo/bell.oga &')
```

### 4. Station Setup Checklist
```
[ ] Clean pogo pins with isopropyl alcohol
[ ] Test continuity of all pogo contacts
[ ] Verify USB connection stable
[ ] Pre-build firmware
[ ] Clear flash-log.txt or backup previous run
[ ] Have spare USB cable ready
[ ] Have spare pogo pin set ready
[ ] Label stations (if using multiple)
[ ] Set up comfortable chair (you'll be here a while!)
```

## Parts to Order NOW for 400-Board Run

### Critical Spares:
```
Qty  Part Number              Description                    Unit Price
---  ----------------------   ----------------------------   ----------
24   Mill-Max 0906-series     Pogo pins (2 complete sets)    $2-3
2    Amphenol 10118192-0001LF Micro USB connector            $2
1    CP2102N-A01-GQFN20       USB-UART bridge IC             $3
2    USB-A to Micro-B cable   Programmer connection          $5

Total: ~$80 for peace of mind during 400-board run
```

### Nice-to-Have:
```
Qty  Part Number              Description                    Unit Price
---  ----------------------   ----------------------------   ----------
2    Toggle clamps            For fixture (McMaster)         $10
4    M3 shoulder screws       Alignment pins                 $2
1    3mm acrylic sheet        Alignment plate material       $15

Total: ~$50 for mechanical improvements
```

## Troubleshooting Guide for Production

### Symptom: Flash fails intermittently
**Likely cause:** Pogo pin contact degradation
**Fix:** Clean pins with isopropyl alcohol; if persists, replace

### Symptom: Board detected but flash immediately fails
**Likely cause:** ICSP not contacting properly
**Fix:** Check board alignment; inspect ICSP pogo pins for damage

### Symptom: Verification fails but flash succeeds
**Likely cause:** UART connection intermittent
**Fix:** Check RXD/TXD pogo pins; verify baud rate (38400)

### Symptom: USB device disconnects during flash
**Likely cause:** USB cable/port issue; power draw too high
**Fix:** Use different USB port (preferably USB 3.0); shorter cable

### Symptom: Slow flash times (>30s total)
**Likely cause:** USB connection negotiation; baud rate issues
**Fix:** Use specified port with --port flag; check USB cable quality

## Long-Term: Next Jig Revision Ideas

For future production runs (or if you're designing v3 of the jig):

1. **ZIF Socket Approach**
   - Use spring-loaded ZIF (Zero Insertion Force) socket
   - Board drops in, lever closes
   - Guaranteed contact every time
   - Example: 3M Textool ZIF sockets

2. **Bed-of-Nails Fixture**
   - Custom PCB with pogo pins at exact EasyPiezi test point locations
   - Pneumatic or lever actuator
   - Professional production quality
   - Cost: ~$300-500 to design and build

3. **Edge Connector Hybrid**
   - Card-edge connector for UART/power
   - Separate ICSP pogo pins or header
   - Fastest insertion/removal
   - Requires EasyPiezi redesign with edge connector

4. **Automated Test Stand**
   - Motorized actuator
   - Barcode scanner for tracking
   - Automatic pass/fail sorting
   - Raspberry Pi control system
   - Cost: ~$500-1000 DIY

## Summary Recommendations for THIS Run

**Must do:**
1. Order spare pogo pins (Priority 1 parts)
2. Implement contact verification in software (2 hours work)
3. Pre-build firmware
4. Create maintenance schedule (clean pins every 50 boards)

**Should do:**
5. Create simple alignment fixture (Priority 3 Option B: 3D print)
6. Add cycle counter to software
7. Set up dual workstation if possible

**Nice to have:**
8. Audio feedback enhancement
9. Keyboard macro for automation

**Estimated prep time:** 4-6 hours
**Estimated cost:** $80-130
**Time saved:** 0.5-2 hours on 400-board run
**Failure reduction:** 50-75% fewer flash failures

Good luck with your production run! 🚀
