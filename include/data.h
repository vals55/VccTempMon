// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include "config.h"
struct Measurements {
    float voltage;
    float temp;
    uint16_t count;
};

struct Data {
    BoardConfig conf;
    Measurements data;
};