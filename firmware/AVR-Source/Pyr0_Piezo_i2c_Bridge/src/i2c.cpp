#include "i2c.h"
#include "Arduino.h"
#include "Wire.h"

uint16_t read16() {
  return Wire.read() << 8 | Wire.read();
}

uint32_t read32() {
  return (uint32_t)Wire.read() << 24 | (uint32_t)Wire.read() << 16 | Wire.read() << 8 | Wire.read();
}

void write(uint8_t cmd) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(cmd);
  Wire.endTransmission();
}

void write(uint8_t cmd, uint16_t value) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(cmd);
  Wire.write(value >> 8);
  Wire.write(value);
  Wire.endTransmission();
}

void write(uint8_t cmd, uint32_t value) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(cmd);
  Wire.write(value >> 24);
  Wire.write(value >> 16);
  Wire.write(value >> 8);
  Wire.write(value);
  Wire.endTransmission();
}

config_t requestConfig() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(CMD_CONFIG);
  Wire.endTransmission();

  uint8_t bytes = Wire.requestFrom(ADDRESS, 255);
  Serial.println(bytes);

  config_t config;
  config.GAIN_FACTOR = read16();
  config.followerThrs = read16();
  config.compThrs = read16();
  config.LOOP_DUR = read16();
  config.TRG_DUR = read16();
  config.Hyst = read16();
  config.LOGIC = read16();
  config.PZDET = read16();
  config.voltMeterConstant = read32();
  config.version = Wire.readString();

  return config;
}

state_t requestState() {
  Wire.beginTransmission(ADDRESS);
  Wire.write(CMD_STATE);
  Wire.endTransmission();

  uint8_t bytes = Wire.requestFrom(ADDRESS, 10);

  state_t state;
  state.Vin = read16();
  state.VComp = read16();
  state.VFol = read16();
  state.ERR_STATE = read16();
  state.PZ_STATE = read16();
}