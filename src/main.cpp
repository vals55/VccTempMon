// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <rlog.h>
#include "data.h"
#include "http.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define VERSION 0.1f

#define OTA_DISABLE
#ifndef OTA_DISABLE
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

//#define RTC_ENABLE
#ifdef RTC_ENABLE
#include "rtc_utils.h"
#endif

#define WIFI_SSID "private-koptevo-wlan-bgn"
#define WIFI_PASS "gfhfvjy2"
#define URL "http://home.shokurov.ru/vcc"
#define BOARD_LED 2

Data data;
DynamicJsonDocument json_data(JSON_BUFFER);
uint8_t attempt = 0;
uint32_t raw = 0;
float voltage = 0.0;

#define ONE_WIRE_BUS 4                // Пин подключения OneWire шины, 0 (D2)
OneWire oneWire(ONE_WIRE_BUS);        // Подключаем бибилотеку OneWire
DallasTemperature sensors(&oneWire);  // Подключаем бибилотеку DallasTemperature
DeviceAddress deviceAddress;
uint8_t deviceCount = 0;
float temp = 0.0f;
String resetReason;
bool wakeup = false;

void setup() {

  #ifdef DEBUG_INFO
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  #endif
  rlog_i("info", "Boot ok");
  
  resetReason = ESP.getResetReason();

  if (resetReason == "Deep-Sleep Wake") {
    wakeup = false;
  }
  else if (resetReason == "Power On") {
    wakeup = false;
  }
  else {
    wakeup = true;
  }
  rlog_i("info", "Reset reason: >%s< wakeup = %d", resetReason.c_str(), wakeup);

  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, LOW);
  
  // get temperature
  #define METHOD1
  sensors.begin();
  deviceCount = sensors.getDeviceCount();
  rlog_i("info", "Device count = %d", deviceCount);
  if (deviceCount) {
  #ifdef METHOD1  
    if (sensors.getAddress(deviceAddress, 0)) {
  #else  
    oneWire.reset_search();
    if (oneWire.search(deviceAddress)) {
  #endif
      rlog_i("info", "Device address = %d", deviceAddress);

      sensors.setResolution(deviceAddress, 10);
      sensors.requestTemperatures();
      temp = sensors.getTempC(deviceAddress);
      data.temp = temp;
      rlog_i("info", "Temperature = %f °C", temp);
    }
  }

  uint16_t count = 0;
#ifdef RTC_ENABLE
  bool rc = false;

  rc = rtc_read(&count);
  rlog_i("info", "RTC read count: %d rc = %d",  count, rc);
  count++;
  rc = rtc_write(&count);
  rlog_i("info", "RTC write count: %d wrc = %d",  count, rc);
#endif  
  data.count = count;

// get voltage
  pinMode(A0, OUTPUT);
  raw = analogRead(A0);
  voltage = raw / 1049.645;
  voltage = voltage * 1.0;
  data.voltage = voltage;

// try to connect
  attempt = 32;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED && attempt--) {
    rlog_i("info", "Wait while WiFi attempt = %d...", 32 - attempt);
    delay(500);
  }

  if(WiFi.status() == WL_CONNECTED) {
    data.version = VERSION;
    getJSONData(data, json_data);
    sendHTTP(URL, json_data);
  
  #ifndef OTA_DISABLE
    ArduinoOTA.begin();
  #endif
  }

//  ESP.deepSleep(1.8e9);
}

void loop() {
  if (!wakeup) {
    delay (1000);
    rlog_i("info", "System is going to sleep...");
    ESP.deepSleep (1.8e9);
  }
  
  #ifndef OTA_DISABLE
  ArduinoOTA.handle();
  #endif
}
