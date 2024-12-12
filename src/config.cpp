// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include <IPAddress.h>
#include "config.h"
#include <rLog.h>

extern EEPROMBuff<BoardConfig> storage;

bool testConfig(BoardConfig &conf) {
    rlog_i("info", "Test Config...");
    
    BoardConfig test;
    return storage.get(test);
}

void storeConfig(const BoardConfig &conf) {

    storage.add(conf);
}

bool loadConfig(BoardConfig &conf) {
    rlog_i("info", "Loading Config...");
    
    if (storage.get(conf)) {

        rlog_i("info", "EEPROM restore config");
        conf.mqtt_host[MQTT_HOST_LEN - 1] = 0;
        conf.mqtt_login[MQTT_LOGIN_LEN - 1] = 0;
        conf.mqtt_password[MQTT_PASSWORD_LEN - 1] = 0;
        conf.mqtt_topic[MQTT_TOPIC_LEN - 1] = 0;
        conf.mqtt_discovery_topic[MQTT_TOPIC_LEN - 1] = 0;
        conf.stat_host[STAT_HOST_LEN - 1] = 0;
        conf.ntp_server[NTP_HOST_LEN - 1] = 0;
        return true;
    } else {
        // Конфигурация не была сохранена в EEPROM, инициализируем с нуля
        rlog_i("info", "EEPROM NEW config");

        conf.version = VERSION;
        String default_topic = String(MQTT_DEFAULT_TOPIC_PREFIX) + "/" + String(ESP.getChipId()) + "/";
        strncpy0(conf.mqtt_topic, default_topic.c_str(), default_topic.length() + 1);
        conf.mqtt_port = MQTT_DEFAULT_PORT;

        conf.mqtt_auto_discovery = MQTT_AUTO_DISCOVERY;
        String discovery_topic(HA_DISCOVERY_TOPIC);
        strncpy0(conf.mqtt_discovery_topic, discovery_topic.c_str(), discovery_topic.length() + 1);

        String stat_host_url(STAT_HOST_URL);
        strncpy0(conf.stat_host, stat_host_url.c_str(), stat_host_url.length() + 1);

        strncpy0(conf.ntp_server, DEFAULT_NTP_SERVER, sizeof(DEFAULT_NTP_SERVER) + 1);
        conf.tz = DEFAULT_TZ;
        
        conf.ip = 0;
        IPAddress network_gateway;
        network_gateway.fromString(DEFAULT_GATEWAY);
        conf.gateway = network_gateway;
        IPAddress network_mask;
        network_mask.fromString(DEFAULT_MASK);
        conf.mask = network_mask;
        conf.wifi_channel = 0;
        conf.wifi_phy_mode = 0;
        conf.wifi_bssid[0] = 0;
        return false;
    }
}

