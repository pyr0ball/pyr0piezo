# Batch Flashing Guide for Pyr0-Piezo Production Runs

This guide explains how to use the automated batch flashing script to program multiple Pyr0-Piezo sensor boards efficiently.

## Prerequisites

1. **Python 3** with `pyserial`:
   ```bash
   pip install pyserial
   ```

2. **PlatformIO Core** (you likely already have this via VSCode):
   ```bash
   # Check if installed
   pio --version

   # If not, install via pip
   pip install platformio
   ```

3. **Programming Jig** with:
   - ICSP connection (2x3 pin header) for bootloader
   - UART connection (TX/RX test pads) for firmware
   - Power connection

## Quick Start

### Basic Usage (Auto-detect boards)

```bash
cd scripts
python batch-flash.py --total 400
```

This will:
1. Wait for you to connect a board
2. Detect when connected
3. Wait for you to press ENTER
4. Flash bootloader via ICSP
5. Flash firmware via UART
6. Verify firmware is running
7. Beep when complete
8. Track progress in `flash-log.txt`

### Specify Serial Port (Faster if you don't change cables)

```bash
python batch-flash.py --port /dev/ttyUSB0 --total 400
```

### Skip Bootloader (Boards already have bootloader)

```bash
python batch-flash.py --skip-bootloader --total 400
```

### Use Specific Firmware

```bash
python batch-flash.py --firmware ../firmware/Compiled-Firmware/pyr0-piezo-firmware_v2.3.2.hex
```

## Workflow

### Recommended Production Flow

1. **Setup:**
   ```bash
   cd /Library/Development/devl/pyr0piezo/scripts
   python batch-flash.py --total 400
   ```

2. **For each board:**
   - Insert board into programming jig
   - Ensure both ICSP and UART connections are made
   - Script will detect the board automatically
   - Press ENTER when prompted
   - Wait for beeps (2 short = success, 1 long = failure)
   - Remove board
   - Insert next board

3. **If interrupted:**
   - The script logs all progress to `flash-log.txt`
   - Simply re-run the same command - it will resume from where you left off

## Script Options

```
--total NUM             Total boards to flash (default: 400)
--port PORT             Specify serial port (default: auto-detect)
--skip-bootloader       Skip bootloader, only flash firmware
--firmware PATH         Path to custom firmware hex file
--log FILE              Log file path (default: flash-log.txt)
```

## Troubleshooting

### "pyserial not installed"

```bash
pip install pyserial
```

### "PlatformIO not found"

```bash
pip install platformio
# Or use the one from VSCode:
export PATH="$HOME/.platformio/penv/bin:$PATH"
```

### "Bootloader flash failed"

- Check ICSP connections (all 6 pins)
- Verify programmer is working
- Try manually with: `./upload-firmware.sh -i`

### "Firmware flash failed"

- Check UART connections (TX, RX, GND)
- Verify bootloader was flashed successfully
- Check baud rate (should be 38400)

### "Verification failed"

- Board might still be working - check manually with serial terminal
- Try power cycling the board
- Send `CONFIG` command via serial to verify

### Auto-detection not working

- Specify port manually: `--port /dev/ttyUSB0`
- Check `dmesg` or `ls /dev/tty*` to see available ports
- Ensure your jig presents as a serial device when connected

## Progress Tracking

The script creates `flash-log.txt` with entries like:

```
COMPLETED: 2025-01-16 10:23:45 - Board flashed successfully on /dev/ttyUSB0
FAILED: 2025-01-16 10:25:12 - Verification failed on /dev/ttyUSB0
SKIPPED: 2025-01-16 10:26:30 - User skipped board on /dev/ttyUSB0
```

You can review this log to find failed boards and retry them later.

## Advanced: Adding a Physical Button

If you want to add a physical button to trigger flashing instead of pressing ENTER:

1. Connect a button to your computer (e.g., via GPIO on a Raspberry Pi, or USB HID button)

2. Modify the `wait_for_user()` function in `batch-flash.py` to read from your button

3. Example for GPIO button on Raspberry Pi:
   ```python
   import RPi.GPIO as GPIO

   def wait_for_button(self, button_pin=17):
       GPIO.setmode(GPIO.BCM)
       GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

       while GPIO.input(button_pin) == GPIO.HIGH:
           time.sleep(0.1)

       return 'flash'
   ```

## Tips for Speed

1. **Pre-build firmware** to avoid building on first flash:
   ```bash
   cd ../firmware/AVR-Source/Pyr0_Piezo_Sensor_v2.x.x
   pio run -e ATmega328PB
   ```

2. **Use compiled hex** instead of building:
   ```bash
   python batch-flash.py --firmware ../firmware/Compiled-Firmware/pyr0-piezo-firmware_v2.3.2.hex
   ```

3. **Skip bootloader** if boards already have it:
   ```bash
   python batch-flash.py --skip-bootloader
   ```

4. **Optimize jig** to make board insertion/removal faster

5. **Run in parallel** with multiple jigs and computers (each with their own log file)

## Batch Statistics

After running, the script shows:
- Total completed
- Total failed
- Success rate
- Location of log file

Example output:
```
╔════════════════════════════════════════════════════════╗
║                   FINAL SUMMARY                        ║
╚════════════════════════════════════════════════════════╝

Total Completed: 387
Total Failed: 13
Success Rate: 96.7%

Log saved to: flash-log.txt
```

## Next Steps

After flashing all boards:
1. Review `flash-log.txt` for any failures
2. Re-flash failed boards individually
3. Perform final quality check on sample boards
4. Package and ship!

## Questions?

If you encounter issues or want to customize the script, the code is well-commented and easy to modify. Check `batch-flash.py` for implementation details.
