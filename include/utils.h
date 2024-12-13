// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include "config.h"

#define BOARD_LED 2

uint16_t getCRC(const BoardConfig &conf);
String getDeviceName();
String getAppName();
bool isMQTT(const BoardConfig &conf);
bool isStat(const BoardConfig &conf);
bool isNTP(const BoardConfig &conf);
bool isHA(const BoardConfig &conf);
bool isDHCP(const BoardConfig &conf);
void flashLED();
