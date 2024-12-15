// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <Arduino.h>

#define MQTT_PARAM_COUNT 8
#define CHANNEL_COUNT 1
#define NONE -1

static const char s_sensor[] PROGMEM    = "sensor";
static const char s_number[] PROGMEM    = "number";
static const char s_float[] PROGMEM     = "float";
// class
static const char c_total[] PROGMEM         = "total";
static const char c_measurement[] PROGMEM   = "measurement";
// device
static const char d_voltage[] PROGMEM   = "voltage";
static const char d_temp[] PROGMEM      = "temperature";
static const char d_rssi[] PROGMEM      = "signal_strength";
static const char d_timestamp[] PROGMEM = "timestamp";
static const char d_duration[] PROGMEM  = "duration";
static const char d_none[] PROGMEM      = "None";
// names & units
static const char n_voltage[] PROGMEM   = "Voltage";
static const char u_voltage[] PROGMEM   = "V";
static const char n_temp[] PROGMEM      = "Temperature";
static const char u_temp[] PROGMEM      = "°С";
static const char n_rssi[] PROGMEM      = "RSSI";
static const char u_rssi[] PROGMEM      = "dBm";
static const char n_time[] PROGMEM      = "Last seen";
static const char n_mac[] PROGMEM       = "MAC Address";
static const char n_chip[] PROGMEM      = "Chip";
static const char n_send_per[] PROGMEM  = "Send every";
static const char u_min[] PROGMEM       = "min";
static const char n_ip[] PROGMEM        = "IP Address";
// ids
static const char i_voltage[] PROGMEM   = "voltage";
static const char i_temp[] PROGMEM      = "temperature";
static const char i_rssi[] PROGMEM      = "rssi";
static const char i_time[] PROGMEM      = "timestamp";
static const char i_mac[] PROGMEM       = "mac";
static const char i_chip[] PROGMEM      = "chip_id";
static const char i_send_per[] PROGMEM  = "send_period";
static const char i_ip[] PROGMEM        = "ip";
// category
static const char cat_diag[] PROGMEM = "diagnostic";
static const char cat_conf[] PROGMEM = "config";
// icons
static const char icon_voltage[] PROGMEM    = "mdi:flash-outline";
static const char icon_temp[] PROGMEM       = "mdi:thermometer-lines";
static const char icon_rssi[] PROGMEM       = "mdi:wifi";
static const char icon_period[] PROGMEM     = "mdi:timer-cog-outline";
static const char icon_last_seen[] PROGMEM  = "mdi:clock-outline";

static const char *const GENERAL_ENTITIES[][MQTT_PARAM_COUNT] PROGMEM = {
    // type,    name,       sensor_id,  state_class,    dev_class,   unit,       cat,       icon
    // single sensors
    {s_sensor,  n_voltage,  i_voltage,  c_measurement,  d_voltage,   u_voltage,  "",        icon_voltage}, 
    {s_sensor,  n_temp,     i_temp,     c_measurement,  d_temp,      u_temp,     "",        icon_temp},
    {s_sensor,  n_time,     i_time,     "",             d_timestamp, "",         cat_diag,  icon_last_seen},
    {s_number,  n_send_per, i_send_per, "",             d_duration,  u_min,      cat_conf,  icon_period},
    {s_sensor,  n_rssi,     i_rssi,     c_measurement,  d_rssi,      u_rssi,     cat_diag,  icon_rssi},
    {s_sensor,  n_ip,       i_ip,       "",             "",          "",         cat_diag,  ""},
    {s_sensor,  n_mac,      i_mac,      "",             "",          "",         cat_diag,  ""},
    {s_sensor,  n_chip,     i_chip,     "",             "",          "",         cat_diag,  ""}
};

static const char *const CHANNEL_ENTITIES[][MQTT_PARAM_COUNT] PROGMEM = {
    // type,    name,       sensor_id,  state_class,    dev_class,   unit,       cat,       icon
    // sensors with attrs
    {s_sensor, n_voltage, i_voltage, c_measurement,  d_voltage, u_voltage, cat_diag,  ""},
    {s_sensor, n_temp,    i_temp,    c_measurement,  d_temp,    u_temp,    cat_diag,  ""}
};

static const char s_VccTemp[] PROGMEM = "VccTemp Monitor";
static const char *const CHANNEL_NAMES[CHANNEL_COUNT] PROGMEM = {s_VccTemp};

static const char s_classic[] PROGMEM = "Vcc & Temp monitor";
static const char s_mod[] PROGMEM     = "Mod Lite";
static const char *const MODEL_NAMES[] PROGMEM = {s_classic, s_mod};
