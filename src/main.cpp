// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <rlog.h>
#include "data.h"
#include "http.h"
#include "sensor.h"
#include "wifi.h"
#include "config.h"
#include "buffer.h"

#ifndef ESP8266
  #define ESP8266
#endif

#define BOARD_LED 2
#define BUTTON 15
#define BTN_HOLD_SETUP 5000
#define BTN_CLICK 200

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

extern "C" {
#include "user_interface.h"
extern struct rst_info resetInfo;
}

Data data;
DynamicJsonDocument json_data(JSON_BUFFER);
uint8_t attempt = 0;
uint32_t raw = 0;
float voltage = 0.0;

String resetReason;
bool wakeup = false;

EEPROMBuff<BoardConfig> storage(8);

void flashLED() {
  digitalWrite(BOARD_LED, LOW);
  delay(5);
  digitalWrite(BOARD_LED, HIGH);
}

void setupBoard() {
  
  digitalWrite(BOARD_LED, LOW);
  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  wifiSetMode(WIFI_AP_STA);
  delay(1000);

  //startAP(data.conf);

  wifiShutdown();
  
  rlog_i("info", "Restart ESP");
  ESP.restart();
}

void setup() {

#ifdef DEBUG_INFO
  Serial.begin(115200);
  delay(1000);
  Serial.println();
#endif
  rlog_i("info", "Boot ok");

  bool success = loadConfig(data.conf);
  rlog_i("info", "loadConfig = %d", success);
  if (!success) {
    rlog_i("info", "Setup board entering");
    // setupBoard();
  }
  
  switch(resetInfo.reason) {
    // normal startup by power on
    case REASON_DEFAULT_RST:      wakeup = false; break;
    // hardware watch dog reset
    case REASON_WDT_RST:          wakeup = false; break;
    // exception reset, GPIO status won’t change
    case REASON_EXCEPTION_RST:    wakeup = false; break;
    // software watch dog reset, GPIO status won’t change
    case REASON_SOFT_WDT_RST:     wakeup = false; break;
    // software restart ,system_restart , GPIO status won’t change
    case REASON_SOFT_RESTART:     wakeup = false; break;
    // wake up from deep-sleep
    case REASON_DEEP_SLEEP_AWAKE: wakeup = false; break;
    // external system reset
    case REASON_EXT_SYS_RST:      wakeup = true; break;
    default:                      wakeup = false; break;
  }
  resetReason = ESP.getResetReason();
  rlog_i("info", "Reset reason: >%s< wakeup = %d", resetReason.c_str(), wakeup);

  pinMode(BOARD_LED, OUTPUT);
  flashLED();
  
  uint16_t count = 0;
#ifdef RTC_ENABLE
  bool rc = false;

  rc = rtc_read(&count);
  rlog_i("info", "RTC read count: %d rc = %d",  count, rc);
  count++;
  rc = rtc_write(&count);
  rlog_i("info", "RTC write count: %d wrc = %d",  count, rc);
#endif  
  data.data.count = count;

// get voltage
  pinMode(A0, OUTPUT);
  raw = analogRead(A0);
  voltage = raw / 1049.645;
  voltage = voltage * (51.0 + 3.3) / 3.3;
  data.data.voltage = voltage;

// try to connect
  attempt = 32;
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED && attempt--) {
    rlog_i("info", "Wait while WiFi attempt = %d...", 32 - attempt);
    delay(500);
    flashLED();
  }

  if(WiFi.status() == WL_CONNECTED) {
    digitalWrite(BOARD_LED, LOW);
    getTempC(data.data);
    getJSONData(data, json_data);
    sendHTTP(URL, json_data);
  
  #ifndef OTA_DISABLE
    ArduinoOTA.begin();
  #endif
  }
  digitalWrite(BOARD_LED, HIGH);

//  ESP.deepSleep(1.8e9);
}

bool flag = false;
uint32_t btnTimer = 0;
uint32_t waitTimer = 0;
uint32_t secTimer = 0;

void loop() {
  if (!wakeup) {
    delay (1000);
    rlog_i("info", "System is going to sleep...");
    ESP.deepSleep (600e6);  // in sec
  }
  
  #ifndef OTA_DISABLE
  ArduinoOTA.handle();
  #endif

  //button
	bool btnState = digitalRead(BUTTON);
  if (btnState && !flag && millis() - btnTimer > 100) {
    flag = true;
    btnTimer = millis();
    rlog_i("info loop >>>>>", "PRESS");
  }
  
  if (btnState && flag && millis() - btnTimer > BTN_HOLD_SETUP) {
    btnTimer = millis();
    rlog_i("info loop >>>>>", "PRESS HOLD");
    setupBoard();
  }
  
  if (!btnState && flag && millis() - btnTimer > BTN_CLICK) {
    btnTimer = millis();
    ESP.restart();
    rlog_i("info loop >>>>>", "MAKE CLICK");
  }
  
  if (!btnState && flag && millis() - btnTimer > 100) {
    flag = false;
    btnTimer = millis();
    rlog_i("info loop >>>>>", "RELEASE");
  }

  // wait and escape timer
  if (millis() - waitTimer >= 60 * PERIOD_SEC) {
    waitTimer = millis();
    rlog_i("info loop >>>>>", "Game over. Restart.");
    flashLED();
    ESP.restart();
  }
  if (millis() - secTimer >= 1 * PERIOD_SEC) {
    secTimer = millis();
    flashLED();
  }
}
