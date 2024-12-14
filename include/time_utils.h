// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include <time.h>
#include "config.h"

#define NTP_ATTEMPTS 24
#define START_VALID_TIME 1577826000UL   // Wed Jan 01 2020 00:00:00
#define TIME_FORMAT "%FT%T%z"

bool syncTime(BoardConfig &conf);
String getLocalTime();
void getUpTime(uint32_t &start, char* buf);
