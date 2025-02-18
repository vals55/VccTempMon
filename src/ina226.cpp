// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include "ina226.h"

//  REGISTERS
#define INA226_CONFIGURATION              0x00
#define INA226_SHUNT_VOLTAGE              0x01
#define INA226_BUS_VOLTAGE                0x02
#define INA226_POWER                      0x03
#define INA226_CURRENT                    0x04
#define INA226_CALIBRATION                0x05
#define INA226_MASK_ENABLE                0x06
#define INA226_ALERT_LIMIT                0x07
#define INA226_MANUFACTURER               0xFE
#define INA226_DIE_ID                     0xFF

INA226::INA226(const uint8_t address, TwoWire *wire) {
  _address     = address;
  _wire        = wire;
}

bool INA226::begin() {
  if (! isConnected()) return false;
  return true;
}

bool INA226::isConnected() {
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}

uint8_t INA226::getAddress() {
  return _address;
};

float INA226::getBusVoltage() {
  uint16_t val = _readRegister(INA226_BUS_VOLTAGE);
  float voltage = val * 1.25e-3;
  return voltage;
}

uint16_t INA226::_readRegister(uint8_t reg) {
  _wire->beginTransmission(_address);
  _wire->write(reg);
  _wire->endTransmission();

  _wire->requestFrom(_address, (uint8_t)2);
  uint16_t value = _wire->read();
  value <<= 8;
  value |= _wire->read();
  return value;
}
