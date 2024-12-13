// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include "buffer.h"
#include <rLog.h>

#define VERSION 1
#define FIRMWARE_VERSION "0.20"
#define HARDWARE_VERSION "0.10"
#define BRAND_NAME "VccMon"
#define CLASSIC 0
#define LITE 1
#define MODEL CLASSIC

#define SSID_LEN 32
#define PASSW_LEN 64
#define MQTT_HOST_LEN 16  
#define MQTT_LOGIN_LEN 32
#define MQTT_PASSWORD_LEN 32
#define MQTT_TOPIC_LEN 64

#define STAT_HOST_LEN 64
#define STAT_HOST_URL "http://home.shokurov.ru/vcc"

#ifndef DISCOVERY_TOPIC
#define HA_DISCOVERY_TOPIC "homeassistant"
#endif

#define DEFAULT_SLEEP_PERIOD 60

#ifndef MQTT_AUTO_DISCOVERY
#define MQTT_AUTO_DISCOVERY true
#endif

#define MQTT_DEFAULT_TOPIC_PREFIX BRAND_NAME
#define MQTT_DEFAULT_PORT 1883

#define NTP_HOST_LEN 64
#define DEFAULT_GATEWAY "192.168.0.1"
#define DEFAULT_MASK "255.255.255.0"
#define DEFAULT_NTP_SERVER "ru.pool.ntp.org"
#define DEFAULT_TZ 3

//time
#define PERIOD_SEC 1000
#define PERIOD_MIN 60 * 1000
#define PERIOD_MEASUREMENT 1 * 1000
#define PERIOD_CHECK_STATE 1 * 1000
#define PERIOD_CHECK_OTA 24 * 3600 * 1000

struct BoardConfig {
    uint8_t version = VERSION;

    char ssid[SSID_LEN] = {0}; 
    char password[PASSW_LEN] = {0};

    uint16_t sleep_period = 60;

    char     mqtt_host[MQTT_HOST_LEN] = {0};
    uint16_t mqtt_port;
    char     mqtt_login[MQTT_LOGIN_LEN] = {0};
    char     mqtt_password[MQTT_PASSWORD_LEN] = {0};
    char     mqtt_topic[MQTT_TOPIC_LEN] = {0};
    char     mqtt_discovery_topic[MQTT_TOPIC_LEN] = HA_DISCOVERY_TOPIC;
    uint8_t  mqtt_auto_discovery = MQTT_AUTO_DISCOVERY;

    char     stat_host[STAT_HOST_LEN] = {0};

    uint32_t ip;
    uint32_t gateway;
    uint32_t mask;

    char ntp_server[NTP_HOST_LEN] = {0};
    uint16_t tz = 3;

    uint8_t wifi_bssid[6] = {0};
    uint8_t wifi_channel = 0;
    uint8_t wifi_phy_mode = 3;
}; // 471

inline void strncpy0(char *dest, const char *src, const size_t len) {
    strncpy(dest, src, len - 1);
    dest[len - 1] = 0;
}

bool loadConfig(BoardConfig &conf);
bool testConfig(BoardConfig &conf);
void storeConfig(const BoardConfig &conf);
