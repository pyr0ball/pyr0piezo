#ifndef I2C
#define I2C

#define ADDRESS 0x10

#define CMD_GAIN_F 0x00
#define CMD_VFOL 0x01
#define CMD_VCOMP 0x02
#define CMD_LOOP_D 0x03
#define CMD_TRG_D 0x04
#define CMD_HYST 0x05
#define CMD_LOGIC 0x06
#define CMD_PZDET 0x07
#define CMD_CONST 0x08
#define CMD_CONFIG 0x09
#define CMD_ERASE 0x0a
#define CMD_STATE 0x0b
#define CMD_VCCSW 0x0c
#define CMD_VCCADJUST 0x0d

#include "WString.h"

typedef struct {
  uint16_t GAIN_FACTOR;
  uint16_t followerThrs;
  uint16_t compThrs;
  uint16_t LOOP_DUR;
  uint16_t TRG_DUR;
  uint16_t Hyst;
  uint16_t LOGIC;
  uint16_t PZDET;
  uint16_t VCCSW;
  uint32_t voltMeterConstant;
  String version;
} config_t;

typedef struct {
  uint16_t Vin;
  uint16_t VComp;
  uint16_t VFol;
  uint16_t ERR_STATE;
  uint16_t PZ_STATE;
} state_t;

void write(uint8_t cmd);
void write(uint8_t cmd, uint16_t value);
void write(uint8_t cmd, uint32_t value);

config_t requestConfig();
state_t requestState();

#endif