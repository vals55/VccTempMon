// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include "Arduino.h"
#include "Wire.h"

class INA226 {
public:
  //  address between 0x40 and 0x4F
  explicit INA226(const uint8_t address, TwoWire *wire = &Wire);

  bool     begin();
  bool     isConnected();
  uint8_t  getAddress();

  float    getBusVoltage(); 

private:

  uint16_t _readRegister(uint8_t reg);

  uint8_t   _address;
  TwoWire * _wire;
};
