// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "wifi.h"
#include "config.h"
#include "rlog.h"
#include "utils.h"

#define WIFI_CONNECT_ATTEMPTS 5
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

void wifiBegin(BoardConfig &conf) {

  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable sleep, can improve ap stability
  delay(200);
  wifiSetMode(WIFI_STA);
 
  if (conf.wifi_phy_mode) {
    WiFi.setPhyMode((WiFiPhyMode_t)conf.wifi_phy_mode);
  }
  
  // if (!WiFi.getAutoConnect()) {
  //   WiFi.setAutoConnect(true);
  // }

  // if (!WiFi.getAutoReconnect()) {
  //   WiFi.setAutoReconnect(true);
  // }

  // if (!WiFi.getPersistent()) {
  //   WiFi.persistent(true);
  // }
  // rlog_i("info", "WiFi autoconnect done.");

  if (isDHCP(conf)) {
    IPAddress fallback_dns_server;
    
    fallback_dns_server.fromString(DEF_FALLBACK_DNS); 
    WiFi.config(conf.ip, conf.gateway, conf.mask, conf.gateway, fallback_dns_server);
  }

  WiFi.hostname(getDeviceName());

  delay(100); // подождем чтобы проинициализировалась сеть

  if (conf.wifi_channel) {
    WiFi.begin(conf.ssid, conf.password, conf.wifi_channel, conf.wifi_bssid);
  }
  else {
    WiFi.begin(conf.ssid, conf.password);
  }

  WiFi.waitForConnectResult(ESP_CONNECT_TIMEOUT);
}

void wifiShutdown() {
  WiFi.disconnect(true);
  delay(100);
  wifiSetMode(WIFI_OFF);
}

String wifiGetMode() {
  // WiFi.setPhyMode(WIFI_PHY_MODE_11B = 1, WIFI_PHY_MODE_11G = 2, WIFI_PHY_MODE_11N = 3);
  WiFiPhyMode_t m = WiFi.getPhyMode();
  String mode;
  switch (m) {
  case WIFI_PHY_MODE_11B:
    mode = F("B");
    break;
  case WIFI_PHY_MODE_11G:
    mode = F("G");
    break;
  case WIFI_PHY_MODE_11N:
    mode = F("N");
    break;
  default:
    mode = (int)m;
    break;
  }
  return mode;
}

bool wifiConnect(BoardConfig &conf) {
  uint32_t start_time = millis();
  rlog_i("info", "WIFI: Connecting...");
  int attempts = WIFI_CONNECT_ATTEMPTS;
  do {
    wifiBegin(conf);
    if (WiFi.isConnected()) {
      conf.wifi_channel = WiFi.channel(); // сохраняем для быстрого коннекта
      uint8_t *bssid = WiFi.BSSID();
      memcpy((void *)&conf.wifi_bssid, (void *)bssid, sizeof(conf.wifi_bssid)); // сохраняем для быстрого коннекта
      // rlog_i("info", "WIFI: Connected.");
      // rlog_i("info", "WIFI: SSID: %s", WiFi.SSID().c_str());
      // rlog_i("info", "WIFI: Channel: %d", WiFi.channel());
      // rlog_i("info", "WIFI: BSSID: %s", WiFi.BSSIDstr().c_str());
      // rlog_i("info", "WIFI: Time spent %d ms", millis() - start_time);
      return true;
    }
    conf.wifi_channel = 0;
    flashLED();
    rlog_i("info", "WIFI: Connection failed.");
  } while (--attempts);
  
  return false;
}