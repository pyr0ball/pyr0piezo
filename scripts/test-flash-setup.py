#!/usr/bin/env python3
"""
Test script to verify flashing setup before batch production

This script tests:
1. Serial port detection
2. PlatformIO availability
3. Firmware build
4. Single board flash (bootloader + firmware)
5. Verification

Run this BEFORE starting batch production to catch issues early!
"""

import os
import sys
import subprocess
import time
from pathlib import Path

try:
    import serial
    import serial.tools.list_ports
    PYSERIAL_OK = True
except ImportError:
    PYSERIAL_OK = False


def print_header(text):
    print(f"\n{'='*60}")
    print(f"  {text}")
    print('='*60)


def test_pyserial():
    """Test if pyserial is installed"""
    print_header("1. Testing pyserial installation")

    if PYSERIAL_OK:
        print("✓ pyserial is installed")
        return True
    else:
        print("✗ pyserial NOT installed")
        print("  Install with: pip install pyserial")
        return False


def test_platformio():
    """Test if PlatformIO is available"""
    print_header("2. Testing PlatformIO")

    try:
        result = subprocess.run(["pio", "--version"], capture_output=True, text=True, timeout=5)
        if result.returncode == 0:
            print(f"✓ PlatformIO found: {result.stdout.strip()}")
            return True
        else:
            print("✗ PlatformIO not working")
            return False
    except FileNotFoundError:
        print("✗ PlatformIO not found in PATH")
        print("  Install with: pip install platformio")
        print("  Or add VSCode PlatformIO to PATH")
        return False
    except Exception as e:
        print(f"✗ Error checking PlatformIO: {e}")
        return False


def test_serial_ports():
    """Test serial port detection"""
    print_header("3. Testing Serial Port Detection")

    if not PYSERIAL_OK:
        print("✗ Cannot test - pyserial not installed")
        return False

    ports = list(serial.tools.list_ports.comports())

    if ports:
        print(f"✓ Found {len(ports)} serial port(s):")
        for port in ports:
            print(f"  - {port.device}: {port.description}")
        return True
    else:
        print("✗ No serial ports detected")
        print("  Connect your programming jig and try again")
        return False


def test_firmware_files():
    """Test if firmware files exist"""
    print_header("4. Testing Firmware Files")

    repo_root = Path(__file__).parent.parent
    firmware_dir = repo_root / "firmware" / "AVR-Source" / "Pyr0_Piezo_Sensor_v2.x.x"
    compiled_hex = repo_root / "firmware" / "Compiled-Firmware" / "pyr0-piezo-firmware_v2.3.2.hex"
    bootloader_hex = repo_root / "scripts" / "bootloader" / "optiboot_flash_atmega328pb_UART0_38400_8000000L.hex"

    all_ok = True

    # Check firmware directory
    if firmware_dir.exists():
        print(f"✓ Firmware source directory exists: {firmware_dir}")
    else:
        print(f"✗ Firmware source directory NOT found: {firmware_dir}")
        all_ok = False

    # Check compiled firmware
    if compiled_hex.exists():
        print(f"✓ Compiled firmware found: {compiled_hex}")
        print(f"  Size: {compiled_hex.stat().st_size} bytes")
    else:
        print(f"⚠ Compiled firmware not found: {compiled_hex}")
        print("  (Will build from source)")

    # Check bootloader
    if bootloader_hex.exists():
        print(f"✓ Bootloader found: {bootloader_hex}")
        print(f"  Size: {bootloader_hex.stat().st_size} bytes")
    else:
        print(f"✗ Bootloader NOT found: {bootloader_hex}")
        all_ok = False

    return all_ok


def test_build():
    """Test building firmware"""
    print_header("5. Testing Firmware Build")

    repo_root = Path(__file__).parent.parent
    firmware_dir = repo_root / "firmware" / "AVR-Source" / "Pyr0_Piezo_Sensor_v2.x.x"

    print("Building firmware (this may take a minute)...")

    try:
        result = subprocess.run(
            ["pio", "run", "-d", str(firmware_dir), "-e", "ATmega328PB"],
            capture_output=True,
            text=True,
            timeout=120
        )

        if result.returncode == 0:
            print("✓ Firmware built successfully")

            # Check if hex was created
            hex_file = firmware_dir / ".pio" / "build" / "ATmega328PB" / "firmware.hex"
            if hex_file.exists():
                print(f"✓ Firmware hex created: {hex_file}")
                print(f"  Size: {hex_file.stat().st_size} bytes")
                return True
            else:
                print("⚠ Build succeeded but hex file not found")
                return False
        else:
            print("✗ Build failed")
            print("\nError output:")
            print(result.stderr[:500])
            return False

    except subprocess.TimeoutExpired:
        print("✗ Build timed out (>2 minutes)")
        return False
    except Exception as e:
        print(f"✗ Build error: {e}")
        return False


def test_single_flash():
    """Offer to test flashing a single board"""
    print_header("6. Single Board Flash Test")

    print("This step will flash ONE board to verify the complete workflow.")
    print("\nReady to test? This will:")
    print("  1. Flash bootloader via ICSP")
    print("  2. Flash firmware via UART")
    print("  3. Verify firmware is running")

    response = input("\nConnect a test board and press 'y' to continue (or 'n' to skip): ").strip().lower()

    if response != 'y':
        print("Skipping single board test")
        return True

    # Get port
    if not PYSERIAL_OK:
        port = input("Enter serial port (e.g., /dev/ttyUSB0): ").strip()
    else:
        ports = list(serial.tools.list_ports.comports())
        if len(ports) == 1:
            port = ports[0].device
            print(f"Using detected port: {port}")
        else:
            print("\nAvailable ports:")
            for i, p in enumerate(ports):
                print(f"  {i+1}. {p.device} - {p.description}")
            choice = input(f"Select port (1-{len(ports)}): ").strip()
            try:
                port = ports[int(choice)-1].device
            except:
                print("Invalid selection")
                return False

    # Run batch-flash.py for a single board
    print(f"\nFlashing test board on {port}...")

    try:
        result = subprocess.run(
            ["python3", "batch-flash.py", "--port", port, "--total", "1"],
            cwd=Path(__file__).parent,
            timeout=180
        )

        if result.returncode == 0:
            print("\n✓ Single board flash test PASSED")
            return True
        else:
            print("\n✗ Single board flash test FAILED")
            return False

    except subprocess.TimeoutExpired:
        print("\n✗ Flash test timed out")
        return False
    except Exception as e:
        print(f"\n✗ Flash test error: {e}")
        return False


def main():
    print("""
╔════════════════════════════════════════════════════════╗
║   Pyr0-Piezo Batch Flash Setup Test                   ║
║   Run this before starting production!                ║
╚════════════════════════════════════════════════════════╝
""")

    results = []

    # Run all tests
    results.append(("pyserial", test_pyserial()))
    results.append(("PlatformIO", test_platformio()))
    results.append(("Serial Ports", test_serial_ports()))
    results.append(("Firmware Files", test_firmware_files()))
    results.append(("Firmware Build", test_build()))

    # Optional single flash test
    if all(r[1] for r in results):
        results.append(("Single Flash Test", test_single_flash()))

    # Summary
    print_header("TEST SUMMARY")

    all_passed = True
    for test_name, passed in results:
        status = "✓ PASS" if passed else "✗ FAIL"
        print(f"{status:8} - {test_name}")
        if not passed:
            all_passed = False

    print("\n" + "="*60)

    if all_passed:
        print("""
✓ ALL TESTS PASSED!

You're ready to start batch production. Run:

    python batch-flash.py --total 400

Good luck with your production run!
""")
        return 0
    else:
        print("""
✗ SOME TESTS FAILED

Please fix the issues above before starting batch production.
Common fixes:

  - Install pyserial: pip install pyserial
  - Install PlatformIO: pip install platformio
  - Check serial connections
  - Verify firmware files exist

Need help? Check BATCH_FLASHING_GUIDE.md
""")
        return 1


if __name__ == "__main__":
    sys.exit(main())
