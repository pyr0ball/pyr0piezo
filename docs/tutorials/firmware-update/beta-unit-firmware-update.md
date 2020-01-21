# Pyr0-piezo Beta Unit Firmware Update

The firmware for this project can be updated using one of three methods, VS Code, Arduino IDE, or avrdude.

Please be sure you've already [wired up your sensor's UART port](../../schematics/other/uart-programmer-wiring.md)

## VS Code (Preferred Application)

### Installing dependencies

1. Install [VS Code from Micosoft's official page](https://code.visualstudio.com/download)
2. Use the "Extensions" menu to install PlatformIO.
3. (Optional) Install extra plugins to assist in development:
    - GitLense
    - markdownlint
    - Markdown Preview Enhanced
    - C/C++
    - C++ Intellisense

### Configuration

1. In PIO Home, open the firmware folder:
    - `~/pyr0piezo/firmware/AVR-Source/Pyr0_Piezo_sensor_v2.x.x`
2. Make certain you're on the latest `develop` branch
    - In GitLense, under the "Repositories" menu, expand the "Branches" menu
    - Right click on `develop` and select "Switch to Branch"
    - Back at the top of the "Repositories" menu, hover the mouse over `Pyr0_Piezo_Sensor_v2.x.x` and click first on `fetch`, then on `pull`

### Upload

1. Make any desired adjustments to default values in `src/pP_config.h`
2. Click the "PlatformIO: Upload" button found in the bottom bar of the VS Code window
3. Verify Serial connection by opening the Serial Monitor and typing `CONFIG` or `STATE`. You should get a response.

## Arduino IDE

1. Install [Arduino IDE from Arduino's official page](https://www.arduino.cc/en/main/software)
2. Under File > Preferences > Additional Board Manager URL's, Add:
    - `https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json`

## avrdude

#### Windows

1. Install a bash-like terminal. Choose from a number of options:
    - [Windows Subsystem for Linux]
    - [CygWin]
    - [Mingw64]
    - [Git Bash (Comes bundled with git)]
2. Download the [avrdude binaries]
3. Extract the binaries somewhere your terminal can find them

#### Linux

##### Ubuntu/Debian Distro's

```bash
sudo apt update && sudo apt install -y avrdude
```

##### Fedora/RHEL/CentOS Distros

```bash
sudo yum install -y avrdude
```
