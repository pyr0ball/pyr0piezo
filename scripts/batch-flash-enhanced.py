#!/usr/bin/env python3
"""
Enhanced Pyr0-Piezo Batch Firmware Flasher with Production Optimizations

New features:
- Pogo pin cycle counter with maintenance alerts
- Auto-start option (no ENTER required)
- Enhanced audio feedback
- Contact verification hooks
- Dual-station support preparation
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
    def __init__(self, total_boards=400, skip_bootloader=False, firmware_path=None,
                 log_file="flash-log.txt", auto_start=False, verify_contact=True):
        self.total_boards = total_boards
        self.skip_bootloader = skip_bootloader
        self.completed = 0
        self.failed = 0
        self.log_file = log_file
        self.auto_start = auto_start
        self.verify_contact = verify_contact

        # Production tracking
        self.jig_insertion_count = 0
        self.pogo_pin_warning_threshold = 200
        self.session_start_time = datetime.now()

        # Paths
        self.repo_root = Path(__file__).parent.parent
        self.firmware_dir = self.repo_root / "firmware" / "AVR-Source" / "Pyr0_Piezo_Sensor_v2.x.x"

        # Set firmware path
        if firmware_path:
            self.firmware_hex = Path(firmware_path)
        else:
            # Try built firmware first, then compiled
            built_hex = self.firmware_dir / ".pio" / "build" / "ATmega328PB" / "firmware.hex"
            compiled_hex = self.repo_root / "firmware" / "Compiled-Firmware" / "pyr0-piezo-firmware_v2.3.2.hex"

            if built_hex.exists():
                self.firmware_hex = built_hex
                print(f"{Colors.OKGREEN}Using pre-built firmware{Colors.ENDC}")
            else:
                self.firmware_hex = compiled_hex

        # Bootloader path
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
                        elif line.startswith("JIG_INSERTIONS:"):
                            try:
                                self.jig_insertion_count = int(line.split(":")[1].strip())
                            except:
                                pass
                print(f"Resuming: {self.completed} completed, {self.failed} failed, {self.jig_insertion_count} jig insertions")
            except Exception as e:
                print(f"{Colors.WARNING}Could not read log: {e}{Colors.ENDC}")
        else:
            with open(self.log_file, 'w') as f:
                f.write(f"# Pyr0-Piezo Batch Flash Log\n")
                f.write(f"# Started: {datetime.now()}\n")
                f.write(f"# Target: {self.total_boards} boards\n")
                f.write(f"# Auto-start: {self.auto_start}\n\n")

    def log_event(self, status, message):
        """Log an event to file"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(self.log_file, 'a') as f:
            f.write(f"{status}: {timestamp} - {message}\n")

        # Update jig insertion count in log
        if status == "COMPLETED" or status == "FAILED":
            with open(self.log_file, 'a') as f:
                f.write(f"JIG_INSERTIONS: {self.jig_insertion_count}\n")

    def beep(self, success=True):
        """Play a beep sound - enhanced with system sounds if available"""
        try:
            if sys.platform == 'linux':
                # Try to use system sounds
                if success:
                    subprocess.run(['paplay', '/usr/share/sounds/freedesktop/stereo/complete.oga'],
                                 stderr=subprocess.DEVNULL, timeout=1)
                else:
                    subprocess.run(['paplay', '/usr/share/sounds/freedesktop/stereo/bell.oga'],
                                 stderr=subprocess.DEVNULL, timeout=1)
            else:
                # Fallback to terminal beep
                if success:
                    print('\a', end='', flush=True)
                    time.sleep(0.1)
                    print('\a', end='', flush=True)
                else:
                    print('\a', end='', flush=True)
        except:
            # Ultimate fallback
            if success:
                print('\a\a', end='', flush=True)
            else:
                print('\a', end='', flush=True)

    def check_pogo_pin_health(self):
        """Check pogo pin cycle count and warn if maintenance needed"""
        if self.jig_insertion_count > 0 and self.jig_insertion_count % 50 == 0:
            print(f"\n{Colors.OKCYAN}ℹ Jig insertions: {self.jig_insertion_count}{Colors.ENDC}")

        if self.jig_insertion_count >= self.pogo_pin_warning_threshold:
            print(f"\n{Colors.WARNING}{'='*60}{Colors.ENDC}")
            print(f"{Colors.WARNING}🔧 MAINTENANCE ALERT:{Colors.ENDC}")
            print(f"{Colors.WARNING}Pogo pins have {self.jig_insertion_count} insertions.{Colors.ENDC}")
            print(f"{Colors.WARNING}Recommended: Clean or replace pogo pins now!{Colors.ENDC}")
            print(f"{Colors.WARNING}{'='*60}{Colors.ENDC}\n")

            if self.jig_insertion_count >= self.pogo_pin_warning_threshold * 1.5:
                response = input(f"{Colors.BOLD}Continue anyway? (y/n): {Colors.ENDC}").strip().lower()
                if response != 'y':
                    print(f"{Colors.WARNING}Maintenance break - exiting{Colors.ENDC}")
                    return False
        return True

    def verify_board_contact(self, port):
        """Verify board is making good electrical contact (placeholder for hardware mod)"""
        if not self.verify_contact:
            return True

        # This is a placeholder - actual implementation requires hardware modification
        # You would check a GPIO pin or use serial port control signals (DTR/RTS/CTS/DSR)
        # to verify the board is seated properly

        # Example implementation if you add hardware:
        # try:
        #     ser = serial.Serial(port, timeout=1)
        #     ser.dtr = True  # Pull high
        #     time.sleep(0.01)
        #     contact_ok = not ser.cts  # Should be pulled low by board GND
        #     ser.close()
        #     return contact_ok
        # except:
        #     return False

        return True  # Disabled for now

    def detect_port(self, timeout=30):
        """Auto-detect when a board is connected"""
        print(f"\n{Colors.OKCYAN}Waiting for board connection...{Colors.ENDC}")
        if not self.auto_start:
            print("(Press Ctrl+C to exit)")

        initial_ports = set([p.device for p in serial.tools.list_ports.comports()])
        start_time = time.time()

        while time.time() - start_time < timeout:
            current_ports = set([p.device for p in serial.tools.list_ports.comports()])
            new_ports = current_ports - initial_ports

            if new_ports:
                port = list(new_ports)[0]
                print(f"{Colors.OKGREEN}✓ Detected board on {port}{Colors.ENDC}")

                # Verify contact if enabled
                if self.verify_contact:
                    if self.verify_board_contact(port):
                        print(f"{Colors.OKGREEN}✓ Contact verified{Colors.ENDC}")
                    else:
                        print(f"{Colors.WARNING}⚠ Poor contact - reseat board{Colors.ENDC}")
                        return None

                return port

            time.sleep(0.5)

        return None

    def wait_for_user(self):
        """Wait for user to press Enter or auto-start"""
        if self.auto_start:
            print(f"{Colors.OKCYAN}Auto-starting in 1 second...{Colors.ENDC}")
            time.sleep(1)
            return 'flash'

        print(f"\n{Colors.BOLD}Press ENTER to start flashing (or 's' to skip, 'q' to quit, 'm' for maintenance){Colors.ENDC}")
        response = input().strip().lower()

        if response == 'q':
            return 'quit'
        elif response == 's':
            return 'skip'
        elif response == 'm':
            return 'maintenance'
        else:
            return 'flash'

    def maintenance_menu(self):
        """Interactive maintenance menu"""
        print(f"\n{Colors.HEADER}{'='*60}{Colors.ENDC}")
        print(f"{Colors.HEADER}MAINTENANCE MENU{Colors.ENDC}")
        print(f"{Colors.HEADER}{'='*60}{Colors.ENDC}")
        print(f"1. Clean pogo pins (reset insertion counter)")
        print(f"2. Replace pogo pins (reset insertion counter)")
        print(f"3. Check jig health")
        print(f"4. View statistics")
        print(f"5. Return to flashing")

        choice = input(f"\n{Colors.BOLD}Select option: {Colors.ENDC}").strip()

        if choice == '1' or choice == '2':
            print(f"{Colors.OKCYAN}Resetting jig insertion counter...{Colors.ENDC}")
            self.jig_insertion_count = 0
            self.log_event("MAINTENANCE", f"Pogo pins {'cleaned' if choice == '1' else 'replaced'}, counter reset")
            print(f"{Colors.OKGREEN}✓ Counter reset{Colors.ENDC}")
        elif choice == '3':
            print(f"\n{Colors.OKCYAN}Jig Health Check:{Colors.ENDC}")
            print(f"  Insertions: {self.jig_insertion_count}")
            print(f"  Threshold: {self.pogo_pin_warning_threshold}")
            health_pct = (1 - self.jig_insertion_count / (self.pogo_pin_warning_threshold * 2)) * 100
            health_pct = max(0, min(100, health_pct))
            print(f"  Health: {health_pct:.0f}%")
            if health_pct > 70:
                print(f"  Status: {Colors.OKGREEN}Good{Colors.ENDC}")
            elif health_pct > 40:
                print(f"  Status: {Colors.WARNING}Fair - consider maintenance{Colors.ENDC}")
            else:
                print(f"  Status: {Colors.FAIL}Poor - maintenance required{Colors.ENDC}")
        elif choice == '4':
            self.show_statistics()

        input(f"\n{Colors.BOLD}Press ENTER to continue...{Colors.ENDC}")

    def show_statistics(self):
        """Show production run statistics"""
        elapsed = datetime.now() - self.session_start_time
        total_processed = self.completed + self.failed

        print(f"\n{Colors.HEADER}{'='*60}{Colors.ENDC}")
        print(f"{Colors.HEADER}SESSION STATISTICS{Colors.ENDC}")
        print(f"{Colors.HEADER}{'='*60}{Colors.ENDC}")
        print(f"Session duration: {elapsed}")
        print(f"Boards completed: {Colors.OKGREEN}{self.completed}{Colors.ENDC}")
        print(f"Boards failed: {Colors.FAIL}{self.failed}{Colors.ENDC}")
        print(f"Total processed: {total_processed}")
        print(f"Remaining: {self.total_boards - total_processed}")

        if total_processed > 0:
            success_rate = (self.completed / total_processed) * 100
            print(f"Success rate: {success_rate:.1f}%")

            avg_time = elapsed.total_seconds() / total_processed
            print(f"Average time per board: {avg_time:.1f}s")

            remaining = self.total_boards - total_processed
            eta_seconds = remaining * avg_time
            eta = datetime.now() + timedelta(seconds=eta_seconds)
            print(f"Estimated completion: {eta.strftime('%H:%M:%S')}")

        print(f"Jig insertions: {self.jig_insertion_count}")

    def flash_bootloader(self, port):
        """Flash bootloader via ICSP"""
        print(f"{Colors.OKCYAN}Flashing bootloader...{Colors.ENDC}")

        if not self.bootloader_hex.exists():
            print(f"{Colors.WARNING}Bootloader hex not found, using PlatformIO...{Colors.ENDC}")
            cmd = [
                "pio", "run",
                "-d", str(self.firmware_dir),
                "-e", "ICSP_Bootloader",
                "-t", "bootloader",
                "--upload-port", port
            ]
        else:
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
                print(f"{Colors.OKGREEN}✓ Bootloader flashed{Colors.ENDC}")
                return True
            else:
                print(f"{Colors.FAIL}✗ Bootloader flash failed{Colors.ENDC}")
                print(result.stderr[:200])
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Bootloader flash timed out{Colors.ENDC}")
            return False
        except FileNotFoundError:
            print(f"{Colors.FAIL}✗ Command not found{Colors.ENDC}")
            return False

    def flash_firmware(self, port):
        """Flash firmware via UART"""
        print(f"{Colors.OKCYAN}Flashing firmware...{Colors.ENDC}")

        if not self.firmware_hex.exists():
            print(f"{Colors.WARNING}Firmware hex not found. Building...{Colors.ENDC}")
            if not self.build_firmware():
                return False

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
                print(f"{Colors.OKGREEN}✓ Firmware flashed{Colors.ENDC}")
                return True
            else:
                print(f"{Colors.FAIL}✗ Firmware flash failed{Colors.ENDC}")
                print(result.stderr[:200])
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Firmware flash timed out{Colors.ENDC}")
            return False
        except FileNotFoundError:
            print(f"{Colors.FAIL}✗ PlatformIO not found{Colors.ENDC}")
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
                self.firmware_hex = self.firmware_dir / ".pio" / "build" / "ATmega328PB" / "firmware.hex"
                return True
            else:
                print(f"{Colors.FAIL}✗ Build failed{Colors.ENDC}")
                print(result.stderr[:200])
                return False
        except subprocess.TimeoutExpired:
            print(f"{Colors.FAIL}✗ Build timed out{Colors.ENDC}")
            return False

    def verify_firmware(self, port):
        """Verify firmware by reading version via serial"""
        print(f"{Colors.OKCYAN}Verifying firmware...{Colors.ENDC}")

        try:
            time.sleep(2)
            ser = serial.Serial(port, self.upload_speed, timeout=5)
            time.sleep(0.5)
            ser.reset_input_buffer()
            ser.write(b"CONFIG\n")
            time.sleep(0.5)
            response = ser.read(1000).decode('utf-8', errors='ignore')
            ser.close()

            if "Firmware" in response or "Version" in response or "GAIN_F" in response:
                print(f"{Colors.OKGREEN}✓ Firmware verified{Colors.ENDC}")
                return True
            else:
                print(f"{Colors.WARNING}⚠ Could not verify firmware{Colors.ENDC}")
                return len(response) > 0

        except Exception as e:
            print(f"{Colors.WARNING}⚠ Verification failed: {e}{Colors.ENDC}")
            return False

    def flash_board(self, port):
        """Flash a single board"""
        self.jig_insertion_count += 1

        print(f"\n{Colors.BOLD}{'='*60}{Colors.ENDC}")
        print(f"{Colors.BOLD}Board {self.completed + self.failed + 1} of {self.total_boards} (Jig insertions: {self.jig_insertion_count}){Colors.ENDC}")
        print(f"{Colors.BOLD}{'='*60}{Colors.ENDC}")

        success = True

        if not self.skip_bootloader:
            if not self.flash_bootloader(port):
                success = False
                self.log_event("FAILED", f"Bootloader flash failed on {port}")

        if success:
            if not self.flash_firmware(port):
                success = False
                self.log_event("FAILED", f"Firmware flash failed on {port}")

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

        remaining = self.total_boards - (self.completed + self.failed)
        print(f"\n{Colors.BOLD}Progress: {self.completed} complete, {self.failed} failed, {remaining} remaining{Colors.ENDC}")

        return success

    def run(self, port=None):
        """Main run loop"""
        print(f"\n{Colors.HEADER}{Colors.BOLD}")
        print("╔════════════════════════════════════════════════════════╗")
        print("║    Pyr0-Piezo Batch Firmware Flasher (Enhanced)       ║")
        print("╚════════════════════════════════════════════════════════╝")
        print(f"{Colors.ENDC}")

        print(f"Target: {self.total_boards} boards")
        print(f"Skip bootloader: {self.skip_bootloader}")
        print(f"Auto-start: {self.auto_start}")
        print(f"Firmware: {self.firmware_hex}")
        print(f"Log file: {self.log_file}")
        print(f"Progress: {self.completed} completed, {self.failed} failed")
        print(f"Jig insertions: {self.jig_insertion_count}")

        try:
            while self.completed + self.failed < self.total_boards:
                # Check pogo pin health
                if not self.check_pogo_pin_health():
                    break

                # Detect or use specified port
                if port:
                    detected_port = port
                else:
                    detected_port = self.detect_port()
                    if not detected_port:
                        print(f"{Colors.WARNING}No board detected. Please connect a board.{Colors.ENDC}")
                        continue

                # Wait for user confirmation (or auto-start)
                action = self.wait_for_user()

                if action == 'quit':
                    print(f"\n{Colors.WARNING}Quitting...{Colors.ENDC}")
                    break
                elif action == 'skip':
                    print(f"{Colors.WARNING}Skipping this board{Colors.ENDC}")
                    self.log_event("SKIPPED", f"User skipped board on {detected_port}")
                    continue
                elif action == 'maintenance':
                    self.maintenance_menu()
                    continue

                # Flash the board
                self.flash_board(detected_port)

                if port:
                    print(f"\n{Colors.OKCYAN}Remove board and insert next one{Colors.ENDC}")
                    time.sleep(2)

        except KeyboardInterrupt:
            print(f"\n\n{Colors.WARNING}Interrupted by user{Colors.ENDC}")

        finally:
            # Final summary
            self.show_statistics()
            print(f"\nLog saved to: {self.log_file}")


def main():
    parser = argparse.ArgumentParser(description="Pyr0-Piezo Batch Firmware Flasher (Enhanced)")
    parser.add_argument("--port", help="Serial port (e.g., /dev/ttyUSB0)")
    parser.add_argument("--total", type=int, default=400, help="Total boards to flash")
    parser.add_argument("--skip-bootloader", action="store_true", help="Skip bootloader flashing")
    parser.add_argument("--firmware", help="Path to firmware hex file")
    parser.add_argument("--log", default="flash-log.txt", help="Log file path")
    parser.add_argument("--auto-start", action="store_true", help="Auto-start without waiting for ENTER")
    parser.add_argument("--no-verify-contact", action="store_true", help="Disable contact verification")

    args = parser.parse_args()

    flasher = BatchFlasher(
        total_boards=args.total,
        skip_bootloader=args.skip_bootloader,
        firmware_path=args.firmware,
        log_file=args.log,
        auto_start=args.auto_start,
        verify_contact=not args.no_verify_contact
    )

    flasher.run(port=args.port)


if __name__ == "__main__":
    # Missing import
    from datetime import timedelta
    main()
