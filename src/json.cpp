// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include "json.h"

#define JSON_BUFFER 1024
#define JSON_SMALL_BUFFER 256

void getJSONData(const Data &data, DynamicJsonDocument &json_data) {

  JsonObject root = json_data.to<JsonObject>();

  root[F("voltage")]     = data.data.voltage;
  root[F("temperature")] = data.data.temp;
  root[F("rssi")]        = WiFi.RSSI();
  root[F("mac")]         = WiFi.macAddress();
  root[F("ip")]          = WiFi.localIP();
  root[F("count")]       = data.data.count;
  root[F("send_period")] = data.conf.sleep_period;
  root[F("ver")]         = String(data.conf.version);
  root[F("chip_id")]     = ESP.getChipId();
  root[F("freemem")]     = ESP.getFreeHeap();
  root[F("timestamp")]   = getLocalTime();
} 