#include "pP_i2c.hpp"
#include "pP_cmd.h"
#include "pP_i2c_config.h"
#include "pP_volatile.h"
#ifdef __AVR_ATmega328P__
  #include <Wire.h>
#endif
#ifdef __AVR_ATmega328PB__
  #include <Wire1.h>
#endif

uint8_t command;
uint32_t value;

void i2cWrite(uint8_t *buffer, int offset, int data) {
  buffer[offset] = (uint8_t)(data >> 8);
  buffer[offset + 1] = (uint8_t)data;
}

void i2cWrite(uint8_t *buffer, int offset, long data) {
  buffer[offset] = (uint8_t)(data >> 24);
  buffer[offset + 1] = (uint8_t)(data >> 16);
  buffer[offset + 2] = (uint8_t)(data >> 8);
  buffer[offset + 3] = (uint8_t)data;
}

void i2cReportConfig() {
  uint8_t length = 20 + sizeof(PP_VERSION) - 1;
  if (length > 32) {
    length = 32;
  }
  uint8_t buffer[length];
  i2cWrite(buffer, 0, GAIN_FACTOR);
  i2cWrite(buffer, 2, followerThrs);
  i2cWrite(buffer, 4, compThrs);
  i2cWrite(buffer, 6, LOOP_DUR);
  i2cWrite(buffer, 8, TRG_DUR);
  i2cWrite(buffer, 10, Hyst);
  i2cWrite(buffer, 12, LOGIC);
  i2cWrite(buffer, 14, PZDET);
  i2cWrite(buffer, 16, VCCSW);
  i2cWrite(buffer, 18, voltMeterConstant);
  memcpy(buffer + 22, PP_VERSION, length - 22);
  #ifdef __AVR_ATmega328P__
  Wire.write(buffer, length);
  #endif
  #ifdef __AVR_ATmega328PB__
  Wire1.write(buffer, length);
  #endif
}

void i2cReportState() {
  uint8_t length = 10;
  uint8_t buffer[length];
  i2cWrite(buffer, 0, Vin);
  i2cWrite(buffer, 2, (int)((long)VComp * Vin / 1023));
  i2cWrite(buffer, 4, (int)((long)VFol * Vin / 1023));
  i2cWrite(buffer, 6, ERR_STATE);
  i2cWrite(buffer, 8, PZ_STATE);
  #ifdef __AVR_ATmega328P__
  Wire.write(buffer, length);
  #endif
  #ifdef __AVR_ATmega328PB__
  Wire1.write(buffer, length);
  #endif
}

void i2cReply() {
  switch (command) {
  case CMD_CONFIG:
  case CMD_ERASE:
    i2cReportConfig();
    break;
  case CMD_STATE:
    i2cReportState();
    break;
  default:
    break;
  }
}

void i2cInput(int bytesReceived) {
  for (int a = 0; a < bytesReceived; a++) {
    // Check length of message, drops anything longer than [longBytes]
    if (a == 0) {
      #ifdef __AVR_ATmega328P__
      command = Wire.read();
      #endif
      #ifdef __AVR_ATmega328PB__
      command = Wire1.read();
      #endif
    } else if (a == 1) {
      #ifdef __AVR_ATmega328P__
      value = Wire.read();
      #endif
      #ifdef __AVR_ATmega328PB__
      value = Wire1.read();
      #endif
    } else {
      value = value << 8 | Wire1.read();
    }
  }

  // Parse commands and apply changes or actions
  switch (command) {
  case CMD_GAIN_F:
    updateGainFactor(value);
    break;
  case CMD_VFOL:
    updateVFol(value);
    break;
  case CMD_VCOMP:
    updateVComp(value);
    break;
  case CMD_LOOP_D:
    updateLoopDuration(value);
    break;
  case CMD_TRG_D:
    updateTrigDuration(value);
    break;
  case CMD_HYST:
    updateHysteresis(value);
    break;
  case CMD_LOGIC:
    updateLogic(value);
    break;
  case CMD_PZDET:
    updatePzDet(value);
    break;
  case CMD_CONST:
    updateConstant(value);
    break;
  case CMD_CONFIG:
    break;
  case CMD_ERASE:
    eraseEEPROM();
    break;
  case CMD_STATE:
    break;
  case CMD_VCCSW:
    updateVccSwitch(value);
    break;
  case CMD_VCCADJUST:
    adjustConstant(value);
    break;
  default:
    return;
  }
}

void i2cInit() {
  #ifdef __AVR_ATmega328P__
  Wire.begin(pP_i2c_address);
  Wire.onRequest(i2cReply);
  Wire.onReceive(i2cInput);
  #endif
  #ifdef __AVR_ATmega328PB__
  Wire1.begin(pP_i2c_address);
  Wire1.onRequest(i2cReply);
  Wire1.onReceive(i2cInput);
  #endif
}
