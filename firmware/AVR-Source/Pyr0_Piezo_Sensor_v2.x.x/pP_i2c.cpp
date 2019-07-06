#include "pP_i2c.h"

void pP_i2c::init() {
  Wire.begin(pP_i2c_address)
}

void pP_i2c::i2cInput(int bytesReceived) {
  for (int a = 0; a < bytesReceived; a++) {
    if (a < maxBytes) {
      cmdRcvd[a] = Wire.receive();
    }
    else {
      longRcvd[a] = Wire.receive();
    }
  }
  if (bytesReceived == 1 && (cmdRcvd[0] < regMapSize)) {
    return;
  }
  if (bytesReceived == 1 && (cmdRcvd[0] >= regMapSize)) {
    cmdRcvd[0] = 0x00;
    return;
  }
  switch (cmdRcvd[0]) {
    case 0x00:
      senseInt = (uint8_t) cmdRcvd[1];
      return;
      break
    case 0x01:
      compInt = (uint8_t) cmdRcvd[1];
      return;
      break;
    case 0x02:
      GAIN_FACTOR = (uint8_t) cmdRcvd[1];
      return;
      break;
    case 0x03:
      Hyst = (uint8_t) cmdRcvd[1];
      return;
      break;
    case 0x04:
      LOOP_DUR = (uint8_t) cmdRcvd[1];
      return;
      break;
    case 0x05:
      TRG_DUR = (uint8_t) cmdRcvd[1];
      return;
      break;
    case 0x06:
      voltMeterConstant = longRcvd[0]*65536+longRcvd[1]*256+longRcvd[2];
      return;
      break;
    default:
      return;
  }
}
