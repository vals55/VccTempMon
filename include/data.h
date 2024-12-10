// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>

struct Data {
    float voltage;
    float temp;
    float version;
    uint16_t count;
};
