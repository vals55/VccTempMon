// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include "config.h"

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
