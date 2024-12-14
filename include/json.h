// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "data.h"
#include "time_utils.h"

#define JSON_BUFFER 1024
#define JSON_SMALL_BUFFER 256

void getJSONData(const Data &data, DynamicJsonDocument &json_data);
