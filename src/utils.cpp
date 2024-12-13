// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include "utils.h"

void flashLED() {
  digitalWrite(BOARD_LED, LOW);
  delay(5);
  digitalWrite(BOARD_LED, HIGH);
}

uint16_t getCRC(const BoardConfig &conf) {
	uint8_t *buf = (uint8_t *)&conf;
	uint16_t crc = 0xffff, poly = 0xa001;
	uint16_t i = 0;
	uint16_t len = sizeof(conf);

	for (i = 0; i < len; i++) {
		crc ^= buf[i];
		for (uint8_t j = 0; j < 8; j++) {
			if (crc & 0x01) {
				crc >>= 1;
				crc ^= poly;
			} else {
				crc >>= 1;
			}
		}
	}
	return crc;
}

String getDeviceName() {
	String deviceName = String(BRAND_NAME) + "-" + ESP.getChipId();
	return deviceName;
}

String getAppName() {
	return getDeviceName() + "-" + String(FIRMWARE_VERSION);
}

bool isMQTT(const BoardConfig &conf) {
#ifndef MQTT_DISABLED
	return conf.mqtt_host[0];
#else
	return false;
#endif
}

bool isStat(const BoardConfig &conf) {
#ifndef STAT_DISABLED
	return conf.stat_host[0];
#else
	return false;
#endif
}

bool isNTP(const BoardConfig &conf) {
	return conf.ntp_server[0];
}

bool isHA(const BoardConfig &conf) {
#ifndef MQTT_DISABLED
	return isMQTT(conf) && conf.mqtt_auto_discovery;
#else
	return false;
#endif
}

bool isDHCP(const BoardConfig &conf) {
	return conf.ip != 0;
}
