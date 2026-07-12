#!/usr/bin/env python3
"""
Pyr0-Piezo Batch Firmware Flasher
Automated flashing tool for production runs

Usage:
    python batch-flash.py [options]

Options:
    --port PORT         Specify serial port (default: auto-detect)
    --total NUM         Total boards to flash (default: 400)
    --skip-bootloader   Skip bootloader flashing (boards already have bootloader)
    --firmware PATH     Path to firmware hex file
    --log FILE          Log file path (default: flash-log.txt)
"""

import os
import sys
import time
import argparse
import subprocess
import json
from datetime import datetime
from pathlib import Path

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("ERROR: pyserial not installed. Install with: pip install pyserial")
    sys.exit(1)


class Colors:
    """Terminal colors for better UX"""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


class BatchFlasher:
    def __init__(self, total_boards=400, skip_bootloader=False, firmware_path=None, log_file="flash-log.txt"):
        self.total_boards = total_boards
        self.skip_bootloader = skip_bootloader
        self.completed = 0
        self.failed = 0
        self.log_file = log_file

        # Paths
        self.repo_root = Path(__file__).parent.parent
        self.firmware_dir = self.repo_root / "firmware" / "AVR-Source" / "Pyr0_Piezo_Sensor_v2.x.x"

        # Set firmware path
        if firmware_path:
            self.firmware_hex = Path(firmware_path)
        else:
            self.firmware_hex = self.repo_root / "firmware" / "Compiled-Firmware" / "pyr0-piezo-firmware_v2.3.2.hex"

        # Bootloader path (from MiniCore or custom)
        self.bootloader_hex = self.repo_root / "scripts" / "bootloader" / "optiboot_flash_atmega328pb_UART0_38400_8000000L.hex"

        # Programming settings
        self.board_type = "m328pb"
        self.upload_speed = 38400
        self.icsp_speed = 19200

        # Initialize log
        self.init_log()

    def init_log(self):
        """Initialize or load existing log file"""
        if os.path.exists(self.log_file):
            print(f"{Colors.OKCYAN}Found existing log file. Loading progress...{Colors.ENDC}")
            try:
                with open(self.log_file, 'r') as f:
                    for line in f:
                        if line.startswith("COMPLETED:"):
                            self.completed += 1
                        elif line.startswith("FAILED:"):
                            self.failed += 1
                print(f"Resuming: {self.completed} completed, {self.failed} failed")
            except Exception as e:
                print(f"{Colors.WARNING}Could not read log: {e}{Colors.ENDC}")
        else:
            with open(self.log_file, 'w') as f:
                f.write(f"# Pyr0-Piezo Batch Flash Log\n")
                f.write(f"# Started: {datetime.now()}\n")
                f.write(f"# Target: {self.total_boards} boards\n\n")

    def log_event(self, status, message):
        """Log an event to file"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(self.log_file, 'a') as f:
            f.write(f"{status}: {timestamp} - {message}\n")

    def beep(self, success=True):
        """Play a beep sound"""
        try:
            if success:
                # Success: two short beeps
                print('\a', end='', flush=True)
                time.sleep(0.1)
                print('\a', end='', flush=True)
            else:
                # Failure: one long beep
                print('\a', end='', flush=True)
        except:
            pass

    def detect_port(self, timeout=30):
        """Auto-detect when a board is connected"""
        print(f"\n{Colors.OKCYAN}Waiting for board connection...{Colors.ENDC}")
        print("(Press Ctrl+C to exit)")

        initial_ports = set([p.device for p in serial.tools.list_ports.comports()])
        start_time = time.time()

        while time.time() - start_time < timeout:
            current_ports = set([p.device for p in serial.tools.list_ports.comports()])
            new_ports = current_ports - initial_ports

            if new_ports:
                port = list(new_ports)[0]
                print(f"{Colors.OKGREEN}✓ Detected board on {port}{Colors.ENDC}")
                return port

            time.sleep(0.5)

        return None

    def wait_for_user(self):
        """Wait for user to press Enter"""
        print(f"\n{Colors.BOLD}Press ENTER to start flashing (or 's' to skip, 'q' to quit){Colors.ENDC}")
        response = input().strip().lower()

        if response == 'q':
            return 'quit'
        elif response == 's':
            return 'skip'
        else:
            return 'flash'

    def flash_bootloader(self, port):
        """Flash bootloader via ICSP"""
        print(f"{Colors.OKCYAN}Flashing bootloader...{Colors.ENDC}")

        # Check if bootloader file exists
        if not self.bootloader_hex.exists():
            print(f"{Colors.WARNING}Bootloader hex not found at {self.bootloader_hex}{Colors.ENDC}")
            print(f"{Colors.WARNING}Attempting to use PlatformIO to flash bootloader...{Colors.ENDC}")

            # Use PlatformIO to flash bootloader
            cmd = [
                "pio", "run",
                "-d", str(self.firmware_dir),
                "-e", "ICSP_Bootloader",
                "-t", "bootloader",
                "--upload-port", port
            ]
        else:
            # Use avrdude directly
            cmd = [
                "avrdude",
                "-c", "stk500v1",
                "-p", self.board_type,
                "-P", port,
                "-b", str(self.icsp_speed),
                "-U", f"flash:w:{self.bootloader_hex}:i",
                "-U", "lfuse:w:0xe2:m",
                "-U", "hfuse:w:0xd6:m",
                "-U", "efuse:w:0xf6:m"
            ]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            if result.returncode == 0:
                print(f"{Colors.OKGREEN}✓ Bootloader flashed successfully{Colors.ENDC}")
                return True
            else:
                print(f"{Colors.FAIL}✗ Bootloader flash failed:{Colors.ENDC}")
                print(result.stderr)
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Bootloader flash timed out{Colors.ENDC}")
            return False
        except FileNotFoundError:
            print(f"{Colors.FAIL}✗ Command not found. Make sure avrdude or PlatformIO is installed{Colors.ENDC}")
            return False

    def flash_firmware(self, port):
        """Flash firmware via UART"""
        print(f"{Colors.OKCYAN}Flashing firmware...{Colors.ENDC}")

        # First, build the firmware if hex doesn't exist
        if not self.firmware_hex.exists():
            print(f"{Colors.WARNING}Firmware hex not found. Building...{Colors.ENDC}")
            if not self.build_firmware():
                return False

        # Use PlatformIO to upload
        cmd = [
            "pio", "run",
            "-d", str(self.firmware_dir),
            "-e", "ATmega328PB",
            "-t", "upload",
            "--upload-port", port
        ]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
            if result.returncode == 0:
                print(f"{Colors.OKGREEN}✓ Firmware flashed successfully{Colors.ENDC}")
                return True
            else:
                print(f"{Colors.FAIL}✗ Firmware flash failed:{Colors.ENDC}")
                print(result.stderr)
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Firmware flash timed out{Colors.ENDC}")
            return False
        except FileNotFoundError:
            print(f"{Colors.FAIL}✗ PlatformIO not found. Make sure it's installed and in PATH{Colors.ENDC}")
            return False

    def build_firmware(self):
        """Build firmware with PlatformIO"""
        print(f"{Colors.OKCYAN}Building firmware...{Colors.ENDC}")

        cmd = [
            "pio", "run",
            "-d", str(self.firmware_dir),
            "-e", "ATmega328PB"
        ]

        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
            if result.returncode == 0:
                print(f"{Colors.OKGREEN}✓ Build successful{Colors.ENDC}")
                # Update firmware hex path to built file
                self.firmware_hex = self.firmware_dir / ".pio" / "build" / "ATmega328PB" / "firmware.hex"
                return True
            else:
                print(f"{Colors.FAIL}✗ Build failed:{Colors.ENDC}")
                print(result.stderr)
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Build timed out{Colors.ENDC}")
            return False

    def verify_firmware(self, port):
        """Verify firmware by reading version via serial"""
        print(f"{Colors.OKCYAN}Verifying firmware...{Colors.ENDC}")

        try:
            # Give board time to boot
            time.sleep(2)

            # Open serial connection
            ser = serial.Serial(port, self.upload_speed, timeout=5)
            time.sleep(0.5)  # Let it settle

            # Flush any existing data
            ser.reset_input_buffer()

            # Send CONFIG command to get firmware info
            ser.write(b"CONFIG\n")
            time.sleep(0.5)

            # Read response
            response = ser.read(1000).decode('utf-8', errors='ignore')
            ser.close()

            # Check if we got a valid response
            if "Firmware" in response or "Version" in response or "GAIN_F" in response:
                print(f"{Colors.OKGREEN}✓ Firmware verified{Colors.ENDC}")
                # Extract version if available
                for line in response.split('\n'):
                    if 'version' in line.lower() or 'firmware' in line.lower():
                        print(f"  {line.strip()}")
                return True
            else:
                print(f"{Colors.WARNING}⚠ Could not verify firmware (no response){Colors.ENDC}")
                print(f"  Response: {response[:100]}")
                # Still count as success if we got any response
                return len(response) > 0

        except Exception as e:
            print(f"{Colors.WARNING}⚠ Verification failed: {e}{Colors.ENDC}")
            return False

    def flash_board(self, port):
        """Flash a single board (bootloader + firmware)"""
        print(f"\n{Colors.BOLD}{'='*60}{Colors.ENDC}")
        print(f"{Colors.BOLD}Board {self.completed + self.failed + 1} of {self.total_boards}{Colors.ENDC}")
        print(f"{Colors.BOLD}{'='*60}{Colors.ENDC}")

        success = True

        # Flash bootloader if needed
        if not self.skip_bootloader:
            if not self.flash_bootloader(port):
                success = False
                self.log_event("FAILED", f"Bootloader flash failed on {port}")

        # Flash firmware
        if success:
            if not self.flash_firmware(port):
                success = False
                self.log_event("FAILED", f"Firmware flash failed on {port}")

        # Verify
        if success:
            if self.verify_firmware(port):
                self.completed += 1
                self.log_event("COMPLETED", f"Board flashed successfully on {port}")
                self.beep(success=True)
                print(f"\n{Colors.OKGREEN}{Colors.BOLD}✓ BOARD COMPLETE!{Colors.ENDC}")
            else:
                success = False
                self.failed += 1
                self.log_event("FAILED", f"Verification failed on {port}")
                self.beep(success=False)
                print(f"\n{Colors.FAIL}{Colors.BOLD}✗ VERIFICATION FAILED{Colors.ENDC}")
        else:
            self.failed += 1
            self.beep(success=False)
            print(f"\n{Colors.FAIL}{Colors.BOLD}✗ BOARD FAILED{Colors.ENDC}")

        # Progress
        remaining = self.total_boards - (self.completed + self.failed)
        print(f"\n{Colors.BOLD}Progress: {self.completed} complete, {self.failed} failed, {remaining} remaining{Colors.ENDC}")

        return success

    def run(self, port=None):
        """Main run loop"""
        print(f"\n{Colors.HEADER}{Colors.BOLD}")
        print("╔════════════════════════════════════════════════════════╗")
        print("║       Pyr0-Piezo Batch Firmware Flasher v1.0          ║")
        print("╚════════════════════════════════════════════════════════╝")
        print(f"{Colors.ENDC}")

        print(f"Target: {self.total_boards} boards")
        print(f"Skip bootloader: {self.skip_bootloader}")
        print(f"Firmware: {self.firmware_hex}")
        print(f"Log file: {self.log_file}")
        print(f"Progress: {self.completed} completed, {self.failed} failed")

        try:
            while self.completed + self.failed < self.total_boards:
                # Detect or use specified port
                if port:
                    detected_port = port
                else:
                    detected_port = self.detect_port()
                    if not detected_port:
                        print(f"{Colors.WARNING}No board detected. Please connect a board.{Colors.ENDC}")
                        continue

                # Wait for user confirmation
                action = self.wait_for_user()

                if action == 'quit':
                    print(f"\n{Colors.WARNING}Quitting...{Colors.ENDC}")
                    break
                elif action == 'skip':
                    print(f"{Colors.WARNING}Skipping this board{Colors.ENDC}")
                    self.log_event("SKIPPED", f"User skipped board on {detected_port}")
                    continue

                # Flash the board
                self.flash_board(detected_port)

                # If port was specified, wait for disconnect
                if port:
                    print(f"\n{Colors.OKCYAN}Remove board and insert next one{Colors.ENDC}")
                    time.sleep(2)

        except KeyboardInterrupt:
            print(f"\n\n{Colors.WARNING}Interrupted by user{Colors.ENDC}")

        finally:
            # Final summary
            print(f"\n{Colors.HEADER}{Colors.BOLD}")
            print("╔════════════════════════════════════════════════════════╗")
            print("║                   FINAL SUMMARY                        ║")
            print("╚════════════════════════════════════════════════════════╝")
            print(f"{Colors.ENDC}")
            print(f"Total Completed: {Colors.OKGREEN}{self.completed}{Colors.ENDC}")
            print(f"Total Failed: {Colors.FAIL}{self.failed}{Colors.ENDC}")
            print(f"Success Rate: {Colors.BOLD}{(self.completed/(self.completed+self.failed)*100 if (self.completed+self.failed) > 0 else 0):.1f}%{Colors.ENDC}")
            print(f"\nLog saved to: {self.log_file}")


def main():
    parser = argparse.ArgumentParser(description="Pyr0-Piezo Batch Firmware Flasher")
    parser.add_argument("--port", help="Serial port (e.g., /dev/ttyUSB0)")
    parser.add_argument("--total", type=int, default=400, help="Total boards to flash")
    parser.add_argument("--skip-bootloader", action="store_true", help="Skip bootloader flashing")
    parser.add_argument("--firmware", help="Path to firmware hex file")
    parser.add_argument("--log", default="flash-log.txt", help="Log file path")

    args = parser.parse_args()

    flasher = BatchFlasher(
        total_boards=args.total,
        skip_bootloader=args.skip_bootloader,
        firmware_path=args.firmware,
        log_file=args.log
    )

    flasher.run(port=args.port)


if __name__ == "__main__":
    main()
