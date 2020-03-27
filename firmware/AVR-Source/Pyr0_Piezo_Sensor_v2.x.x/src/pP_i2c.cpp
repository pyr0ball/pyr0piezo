#ifdef I2C_INPUT

#include "pP_i2c.h"
#include "pP_config.h"
#include <Arduino.h>
#include <Wire.h>

byte registerMap[regMapSize];
byte registerMapTemp[regMapSize - 1];
byte receivedCommands[maxBytes];

pP_i2c::pP_i2c() {
}

void pP_i2c::init() {
  Wire.begin(pP_i2c_address);
  Wire.onRequest(i2cReply);
  Wire.onReceive(i2cInput);
}

void pP_i2c::i2cReportStatus() {
  _i2cResponse = "{"
}

void pP_i2c::i2cReportVersion() {
}

void pP_i2c::i2cReportConfig() {
}

void pP_i2c::i2cReportIdentity() {
}

void pP_i2c::i2cRequestInput() {
}

void pP_i2c::i2cReply() {
  Wire.send()
}

void pP_i2c::i2cInput(int bytesReceived) {
  for (int a = 0; a < bytesReceived; a++) {
    // Check length of message, drops anything longer than [longBytes]
    if (a <= maxBytes) {
      cmdRcvd[a] = Wire.receive();
    }
    elif (a <= longBytes) {
      longRcvd[a] = Wire.receive();
    }
    else {
      Wire.receive(); //
    }
  }

  // Check input command corresponds with register map, set 0x00 if not
  if (bytesReceived == 1 && (cmdRcvd[0] < regMapSize)) {
    return;
  }
  if (bytesReceived == 1 && (cmdRcvd[0] >= regMapSize)) {
    cmdRcvd[0] = 0x00;
    return;
  }

  // Parse commands and apply changes or actions
  switch (cmdRcvd[0]) {
  case 0x00:
    i2cReportStatus();
    return;
    break;
  case 0x01:
    followerInt = (int)cmdRcvd[1];
    return;
    break;
  case 0x02:
    compInt = (int)cmdRcvd[1];
    return;
    break;
  case 0x03:
    GAIN_FACTOR = (int)cmdRcvd[1];
    return;
    break;
  case 0x04:
    Hyst = (int)cmdRcvd[1];
    return;
    break;
  case 0x05:
    LOOP_DUR = (int)cmdRcvd[1];
    return;
    break;
  case 0x06:
    LOGIC = (int)cmdRcvd[1];
    return;
    break;
  case 0x07:
    PZDET = (int)cmdRcvd[1];
    return;
    break;
  case 0x08:
    TRG_DUR = (int)cmdRcvd[1];
    return;
    break;
  case 0x09:
    DEBUG = (int)cmdRcvd[1];
    return;
    break;
  case 0x0a:
    voltMeterConstant = longRcvd[0] * 65536 + longRcvd[1] * 256 + longRcvd[2];
    return;
    break;
  case 0x0b:
    reportVersion();
    return;
    break;
  case 0x0c:
    reportConfig();
    return;
    break;
  case 0x0d:
    reportIdentity();
    return;
    break;
  default:
    return;
  }
}
#endif
