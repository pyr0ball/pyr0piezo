#include "pP_i2c.hpp"
#include "pP_cmd.h"
#include "pP_i2c_config.h"
#include "pP_volatile.h"
#include <Wire.h>

uint8_t command;
uint16_t value;

void i2cWrite(int data) {
  Wire.write(data >> 8);
  Wire.write(data);
}

void i2cWrite(long data) {
  Wire.write(data >> 24);
  Wire.write(data >> 16);
  Wire.write(data >> 8);
  Wire.write(data);
}

void i2cReportConfig() {
  i2cWrite(GAIN_FACTOR);
  i2cWrite(followerThrs);
  i2cWrite(compThrs);
  i2cWrite(LOOP_DUR);
  i2cWrite(TRG_DUR);
  i2cWrite(Hyst);
  i2cWrite(LOGIC);
  i2cWrite(PZDET);
  i2cWrite(voltMeterConstant);
}

void i2cReportState() {
  i2cWrite(Vin);
  i2cWrite((int)((long)VComp * Vin / 1023));
  i2cWrite((int)((long)VFol * Vin / 1023));
  i2cWrite(ERR_STATE);
  i2cWrite(PZ_STATE);
}

void i2cReply() {
  Serial.print("Requested ");
  Serial.println(command);
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
      command = Wire.read();
    } else if (a == 1) {
      value = Wire.read();
    } else if (a == 2) {
      value = value << 8 | Wire.read();
    } else {
      Wire.read(); //
    }
  }

  Serial.print("Command ");
  Serial.print(command);
  Serial.print(" ");
  Serial.println(value);

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
  default:
    return;
  }
}

void i2cInit() {
  Wire.begin(pP_i2c_address);
  Wire.onRequest(i2cReply);
  Wire.onReceive(i2cInput);
}
