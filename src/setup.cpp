// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include <WiFiManager.h>

#include "setup.h"
#include "rlog.h"
#include "utils.h"

WiFiManager wm;
bool shouldSaveConfig = false;

#define SETUP_TIME_SEC 300

void saveConfigCallback () {
  shouldSaveConfig = true;
}

void wifiInfo() {
  // can contain gargbage on esp32 if wifi is not ready yet
  rlog_i("info", "[wifiInfo] WIFI INFO DEBUG");
  rlog_i("info", "[wifiInfo] SAVED: %s", (String)(wm.getWiFiIsSaved() ? "YES" : "NO"));
  rlog_i("info", "[wifiInfo] SSID: %s", (String)wm.getWiFiSSID());
  rlog_i("info", "[wifiInfo] PASS: %s", (String)wm.getWiFiPass());
  rlog_i("info", "[wifiInfo] HOSTNAME: %s", (String)WiFi.getHostname());
}

void startAP(BoardConfig &conf) {

#ifdef WIFI_DEBUG_INFO
  rlog_i("info", "WiFi debug info enabled");
  wm.setDebugOutput(true);
  wm.debugPlatformInfo();

  //reset settings - for testing
  // wm.resetSettings();    
  // wm.erase();
#endif

  if (conf.ssid[0]) {
      struct station_config sconf;
      sconf.bssid_set = 0;
      memcpy(sconf.ssid, conf.ssid, sizeof(sconf.ssid));
      if (conf.password[0]) {
          memcpy(sconf.password, conf.password, sizeof(sconf.password));
      } else {
          sconf.password[0] = 0;
      }    
      wifi_station_set_config(&sconf);
  }

  LongParameter param_sleep_period("sleep_period", "Период отправки показаний, мин.:", conf.sleep_period);
  wm.addParameter(&param_sleep_period);

  FloatParameter param_coeff("coeff", "Поправочный коэфф. вольтметра:", conf.coeff);
  wm.addParameter(&param_coeff);

  WiFiManagerParameter subtitle_mqtt("<h3>MQTT</h3>");
  wm.addParameter(&subtitle_mqtt);

  WiFiManagerParameter param_mqtt_host("mqtt_host", "Адрес брокера:", conf.mqtt_host, MQTT_HOST_LEN-1);
  wm.addParameter(&param_mqtt_host);

  LongParameter param_mqtt_port("mqtt_port", "Порт:", conf.mqtt_port);
  wm.addParameter(&param_mqtt_port);

  WiFiManagerParameter param_mqtt_login("mqtt_login", "Логин:", conf.mqtt_login, MQTT_LOGIN_LEN-1);
  wm.addParameter(&param_mqtt_login);

  WiFiManagerParameter param_mqtt_password("mqtt_password", "Пароль:", conf.mqtt_password, MQTT_PASSWORD_LEN-1);
  wm.addParameter(&param_mqtt_password);

  WiFiManagerParameter param_mqtt_topic("mqtt_topic", "Топик данных:", conf.mqtt_topic, MQTT_TOPIC_LEN-1);
  wm.addParameter(&param_mqtt_topic);

  WiFiManagerParameter param_mqtt_discovery_topic("mqtt_discovery_topic", "Топик Home Assistant:", conf.mqtt_discovery_topic, MQTT_TOPIC_LEN-1);
  wm.addParameter(&param_mqtt_discovery_topic);

  WiFiManagerParameter div_checkbox("<label class='chk-box'>Автоматическое добавление в Home Assistant");
  wm.addParameter(&div_checkbox);
  CheckBoxParameter param_mqtt_auto_discovery("mqtt_auto_discovery", "", conf.mqtt_auto_discovery);
  wm.addParameter(&param_mqtt_auto_discovery);
  WiFiManagerParameter div_end_checkbox("<span class='box'></span></label>");
  wm.addParameter(&div_end_checkbox);

  WiFiManagerParameter subtitle_stat("<h3>Статистика</h3>");
  wm.addParameter(&subtitle_stat);

  WiFiManagerParameter param_stat_host("stat_host", "URL сбора статистики:", conf.stat_host, STAT_HOST_LEN-1);
  wm.addParameter(&param_stat_host);

//extra conf ------------------------------------------------------------------------------------
  WiFiManagerParameter checkbox("<label class='chk-box'>Расширенные настройки<input type='checkbox' id='chbox' name='chbox' onclick='extraConf()'><span class='box'></span></label>");
  wm.addParameter(&checkbox);
  WiFiManagerParameter div_start("<div id='extra_conf' style='display:none'>");
  wm.addParameter(&div_start);

  WiFiManagerParameter subtitle_network("<h3>Сетевые настройки</h3>");
  wm.addParameter(&subtitle_network);

  String mac("<label>MAC адрес: ");
  mac += WiFi.macAddress();
  mac += "</label><br /><br />";
  WiFiManagerParameter label_mac(mac.c_str());
  wm.addParameter(&label_mac);

  IPAddressParameter param_ip("ip", "Статический IP (оставьте 0.0.0.0 для DHCP):", IPAddress(conf.ip));
  wm.addParameter(&param_ip);
  IPAddressParameter param_gateway("gateway", "Шлюз:", IPAddress(conf.gateway));
  wm.addParameter(&param_gateway);
  IPAddressParameter param_mask("mask", "Маска подсети:", IPAddress(conf.mask));
  wm.addParameter(&param_mask);

  WiFiManagerParameter label_phy_mode("<label>Режим работы WiFi:</label>");
  wm.addParameter(&label_phy_mode);
  DropdownParameter dropdown_phy_mode("wifi_phy_mode");
  dropdown_phy_mode.add_option(0, "Авто", conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(1, "11b",  conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(2, "11g",  conf.wifi_phy_mode);
  dropdown_phy_mode.add_option(3, "11n",  conf.wifi_phy_mode);
  wm.addParameter(&dropdown_phy_mode);

  WiFiManagerParameter param_ntp_server("ntp_server", "Сервер времени (NTP):", conf.ntp_server, NTP_HOST_LEN - 1);
  wm.addParameter(&param_ntp_server);

  LongParameter param_tz("tz", "Часовой пояс (разница с UTC):", conf.tz);
  wm.addParameter(&param_tz);

//extra conf end---------------------------------------------------------------------------------
  WiFiManagerParameter div_end("</div>");
  wm.addParameter(&div_end);

  wm.setSaveConfigCallback(saveConfigCallback);

  // set custom channel
  // wm.setWiFiAPChannel(13);
  
  // set AP hidden
  // wm.setAPHidden(true);

  wm.setConfigPortalTimeout(SETUP_TIME_SEC);
  wm.setBreakAfterConfig(true);
  WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  // set custom webserver port, automatic captive portal does not work with custom ports!
  // wm.setHttpPort(8080);

  //bool result = wm.startConfigPortal(getAppName().c_str());
  // if (!wm.autoConnect(getAppName().c_str())) {
  if (!wm.startConfigPortal(getAppName().c_str())) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    // ESP.restart();
    // delay(5000);
  }

#ifdef WIFI_DEBUG_INFO
   wifiInfo();
#endif  
    
  strncpy0(conf.ssid, wm.getWiFiSSID().c_str(), SSID_LEN);
  strncpy0(conf.password, wm.getWiFiPass().c_str(), PASSW_LEN);

  conf.sleep_period = param_sleep_period.getValue();
 
  strncpy0(conf.mqtt_host, param_mqtt_host.getValue(), MQTT_HOST_LEN);
  conf.mqtt_port = param_mqtt_port.getValue();
  strncpy0(conf.mqtt_login, param_mqtt_login.getValue(), MQTT_LOGIN_LEN);
  strncpy0(conf.mqtt_password, param_mqtt_password.getValue(), MQTT_PASSWORD_LEN);
  strncpy0(conf.mqtt_topic, param_mqtt_topic.getValue(), MQTT_TOPIC_LEN);
  strncpy0(conf.mqtt_discovery_topic, param_mqtt_discovery_topic.getValue(), MQTT_TOPIC_LEN);
  conf.mqtt_auto_discovery = param_mqtt_auto_discovery.getValue();

  strncpy0(conf.stat_host, param_stat_host.getValue(), STAT_HOST_LEN);

  conf.ip = param_ip.getValue();
  if (conf.ip) {
    conf.gateway = param_gateway.getValue();
    conf.mask = param_mask.getValue();
  }
  conf.wifi_phy_mode = dropdown_phy_mode.getValue();
  strncpy0(conf.ntp_server, param_ntp_server.getValue(), NTP_HOST_LEN);
  conf.tz = param_tz.getValue();

  if (shouldSaveConfig) {
    storeConfig(conf);
  }
}