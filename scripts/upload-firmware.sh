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
serialdevice=COM3
board=m328pb
lowfuse=0xE2
highfuse=0xD6
exfuse=0xF6
firmwarebin=Pyr0_Piezo_Sensor_m328pb_v2.x.x.hex

# Programming functions
program-icsp() {
  avrdude -c avrisp -p $board -b19200 -P$serialdevice -U lfuse:w:$lowfuse:m -U hfuse:w:$highfuse:m -U efuse:w:$exfuse:m -U flash:w:$firmwarebin -v
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
      b)  board="$2";
        shift ;;
      d)  serialdevice="$2";
        shift ;;
      f)  firmwarebin="$2";
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
