// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>

#define VERSION 7
#define FIRMWARE_VERSION "1.52"
#define HARDWARE_VERSION "0.70"
#define BRAND_NAME "eMeter"
#define CLASSIC 0
#define LITE 1
#define MODEL CLASSIC

#define SSID_LEN 32
#define PASSW_LEN 64
#define MQTT_HOST_LEN 44    //64 хватило бы и 15 для ip адреса, делаем элемент буфера не более 512 вместе с crc
#define MQTT_LOGIN_LEN 32
#define MQTT_PASSWORD_LEN 32
#define MQTT_TOPIC_LEN 64

#define STAT_HOST_LEN 64

#ifndef DISCOVERY_TOPIC
#define HA_DISCOVERY_TOPIC "homeassistant"
#endif

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

    uint16_t mqtt_period = 10;
    char     mqtt_host[MQTT_HOST_LEN] = {0};
    uint16_t mqtt_port;
    char     mqtt_login[MQTT_LOGIN_LEN] = {0};
    char     mqtt_password[MQTT_PASSWORD_LEN] = {0};
    char     mqtt_topic[MQTT_TOPIC_LEN] = {0};
    char     mqtt_discovery_topic[MQTT_TOPIC_LEN] = HA_DISCOVERY_TOPIC;
    uint8_t  mqtt_auto_discovery = MQTT_AUTO_DISCOVERY;

    uint16_t stat_period = 60;
    char     stat_host[STAT_HOST_LEN] = {0};

    uint32_t ip;
    uint32_t gateway;
    uint32_t mask;

    char ntp_server[NTP_HOST_LEN] = {0};
    uint16_t tz = 3;

    uint8_t wifi_bssid[6] = {0};
    uint8_t wifi_channel = 0;
    uint8_t wifi_phy_mode = 3;
};
