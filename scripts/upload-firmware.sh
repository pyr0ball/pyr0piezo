#!/bin/bash

read -r -d '' usage << EOF
upload-firmware.sh [-h] [-b] m328pb [-d] /dev/ttyUSB0||COM3 [-f] firmware.hex [-i] [-u] -- firmware upload script for Pyr0-Piezo AVR boards

Usage:
    -h  show this help text

    -b  Specify a board type. Default is m328pb. Other option is m88p

    -d  Specify a serial device

    -f  Specify a firmware file
    
    -i Program over ICSP
    
    -u Program over UART

EOF

# Default values
serialdevice=/dev/ttyUSB0
board=m328pb
lowfuse=0xE2
highfuse=0xD6
exfuse=0xF6
firmwarebin=../firmware/Compiled-Firmware/pyr0_piezo_firmware_v2.3.2.hex

# Programming functions
program-icsp() {
  #avrdude -c avrisp -p $board -b19200 -P$serialdevice -U lfuse:w:$lowfuse:m -U hfuse:w:$highfuse:m -U efuse:w:$exfuse:m -U flash:w:$firmwarebin -v
  avrdude -Cbootloader/MiniCire-avrdude.conf -v -patmega328pb -cstk500v1 -P$serialdevice -b19200 -e -Ulock:w:0x3f:m -Uefuse:w:0b11110110:m -Uhfuse:w:0xd6:m -Ulfuse:w:0xe2:m 
  avrdude -Cbootloader/MiniCire-avrdude.conf -v -patmega328pb -cstk500v1 -P$serialdevice -b19200 -Uflash:w:bootloader/optiboot_flash_atmega328pb_UART0_38400_8000000L.hex:i -Ulock:w:0x0f:m
  avrdude -Cbootloader/MiniCire-avrdude.conf -v -c stk500v1 -p $board -b19200 -P$serialdevice -U flash:w:$firmwarebin
}

program-uart() {
  avrdude -c stk500v1 -p $board -b19200 -P$serialdevice -U flash:w:$firmwarebin -v
}

# Options parser

while getopts ":b:d:f:i:u" opt
  do
    case ${opt} in
      h)  echo "$usage"
        exit
        ;;
      b)  board="$@";
        shift ;;
      d)  serialdevice="$@";
        shift ;;
      f)  firmwarebin="$@";
        shift ;;
      i)  program-icsp
        shift ;;
      u)  program-uart
        shift ;;
      \?) printf "illegal option: -%s\n" "$OPTARG" >&2
        echo "$usage" >&2
        exit 1
        ;;
    esac
  done
    if [ $OPTIND == 1 ] ; then
      program-icsp
    fi
    shift $((OPTIND - 1))
