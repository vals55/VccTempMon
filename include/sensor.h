// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "data.h"
#include <rlog.h>

#define ONE_WIRE_BUS 4                  // Пин подключения OneWire шины, 0 (D2)
#define METHOD1                         // метод измерений

OneWire oneWire(ONE_WIRE_BUS);          // Подключаем бибилотеку OneWire
DallasTemperature sensors(&oneWire);    // Подключаем бибилотеку DallasTemperature
DeviceAddress deviceAddress;
uint8_t deviceCount = 0;


void getTempC(Measurements &data) {

  data.temp = 0.0;

  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  rlog_i("info", "Device count = %d", deviceCount);
  if (deviceCount) {
#ifdef METHOD1  
    if (sensors.getAddress(deviceAddress, 0)) {
#else  
    oneWire.reset_search();
    if (oneWire.search(deviceAddress)) {
#endif
      rlog_i("info", "Device address = %d", deviceAddress);

      sensors.setResolution(deviceAddress, 9);
      sensors.requestTemperatures();
      data.temp = sensors.getTempC(deviceAddress);
      if (data.temp == 85) {
        delay(200);
        sensors.requestTemperatures();
        data.temp = sensors.getTempC(deviceAddress);
      }
      rlog_i("info", "Temperature = %f °C", data.temp);
    }
  }
}    
