// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <ESP8266WiFi.h>
#include "config.h"

bool wifiConnect(BoardConfig &conf);
void wifiBegin(BoardConfig &conf);
void wifiSetMode(WiFiMode_t wifi_mode);
void wifiShutdown();
String wifiGetMode();
