// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include "config.h"

uint16_t getCRC(const BoardConfig &conf);
String getDeviceName();
String getAppName();
