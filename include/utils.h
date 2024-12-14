// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include "config.h"

#define BOARD_LED 2

#define MAC_STR "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_STR_HEX "%02X%02X%02X%02X%02X%02X"
#define PROTO_HTTPS "https"
#define PROTO_HTTP "http"

uint16_t getCRC(const BoardConfig &conf);
String getDeviceName();
String getAppName();
bool isMQTT(const BoardConfig &conf);
bool isStat(const BoardConfig &conf);
bool isNTP(const BoardConfig &conf);
bool isHA(const BoardConfig &conf);
bool isDHCP(const BoardConfig &conf);
void flashLED();
void removeSlash(String &str);
String getMacAddressHex();
