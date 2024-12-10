// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <ESP8266WiFi.h>
#include <rlog.h>

#define WIFI_CONNECT_ATTEMPTS 2
#define WIFI_SET_MODE_ATTEMPTS 2
#define DEF_FALLBACK_DNS "8.8.8.8"
#define ESP_CONNECT_TIMEOUT 10000UL

void wifiSetMode(WiFiMode_t wifi_mode) {
  if (WiFi.getMode() == wifi_mode) {
    return;
  }

  if (wifi_mode != WIFI_OFF) {
    WiFi.forceSleepWake();
    delay(100);
  }

  uint32_t attempts = WIFI_SET_MODE_ATTEMPTS;
  while (!WiFi.mode(wifi_mode) && attempts--) {
    rlog_i("info", "WIFI: Retry set Mode...");
    delay(100);
  }

  if (wifi_mode == WIFI_OFF) {
    delay(1000);
    WiFi.forceSleepBegin();
    delay(1);
  }
  else {
    delay(30);
  }
}

void wifiShutdown() {
  WiFi.disconnect(true);
  delay(100);
  wifiSetMode(WIFI_OFF);
}
