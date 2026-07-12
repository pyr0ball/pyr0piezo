# Scripts Directory

Utility scripts for Pyr0-Piezo development and production.

## Production Tools

### `batch-flash.py` - Automated Production Flasher

Automates flashing of multiple boards for production runs. Features:
- Auto-detects board connections
- Flashes bootloader + firmware
- Verifies each board
- Progress tracking and logging
- Resume capability

**Quick start:**
```bash
python test-flash-setup.py    # Test setup first!
python batch-flash.py --total 400
```

See [BATCH_FLASHING_GUIDE.md](BATCH_FLASHING_GUIDE.md) for complete documentation.

### `test-flash-setup.py` - Pre-production Test

Tests your flashing setup before starting a batch run. Verifies:
- Python dependencies (pyserial)
- PlatformIO installation
- Serial port detection
- Firmware build process
- Single board flash test

**Always run this before starting production!**

```bash
python test-flash-setup.py
```

## Development Tools

### `upload-firmware.sh` - Manual Firmware Upload

Shell script for manual firmware uploads via ICSP or UART.

```bash
# Upload via UART
./upload-firmware.sh -u -d /dev/ttyUSB0 -f ../firmware/Compiled-Firmware/pyr0-piezo-firmware_v2.3.2.hex

# Program via ICSP (with bootloader)
./upload-firmware.sh -i -d /dev/ttyUSB0 -b m328pb
```

### `deploy-mkdocs.sh` - Documentation Deployment

Builds and deploys the documentation site to GitHub Pages.

```bash
./deploy-mkdocs.sh
```

### `changelog.sh` - Changelog Generator

Generates changelogs from git commits tagged with `#bugfix`, `#featureadd`, or `#docs`.

## Bootloader Files

The `bootloader/` directory contains:
- `optiboot_flash_atmega328pb_UART0_38400_8000000L.hex` - Bootloader for ATmega328PB at 8MHz
- `MiniCore-avrdude.conf` - avrdude configuration for MiniCore boards

## Common Workflows

### First Time Production Setup

```bash
# 1. Install dependencies
pip install pyserial platformio

# 2. Test your setup
python test-flash-setup.py

# 3. Fix any issues found by the test

# 4. Start production run
python batch-flash.py --total 400
```

### Resume Interrupted Production Run

```bash
# The script automatically resumes from flash-log.txt
python batch-flash.py --total 400
```

### Flash Individual Board Manually

```bash
# Via PlatformIO (recommended)
cd ../firmware/AVR-Source/Pyr0_Piezo_Sensor_v2.x.x
pio run -e ATmega328PB -t upload --upload-port /dev/ttyUSB0

# Via shell script
./upload-firmware.sh -u -d /dev/ttyUSB0
```

### Build Documentation Locally

```bash
cd ..
mkdocs serve
# Visit http://localhost:8000
```

## Troubleshooting

### "pyserial not installed"
```bash
pip install pyserial
```

### "PlatformIO not found"
```bash
pip install platformio
# Or add VSCode PlatformIO to PATH
```

### "Permission denied" on serial port
```bash
# Linux: Add yourself to dialout group
sudo usermod -a -G dialout $USER
# Then log out and back in
```

### Flash verification fails
- Check serial connections (TX, RX, GND)
- Try power cycling the board
- Manually verify with: `screen /dev/ttyUSB0 38400` then send `CONFIG`

## Questions?

Check the main repository documentation or ask in the Discord.
