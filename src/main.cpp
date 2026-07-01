// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <rlog.h>
#include <WiFiManager.h>
#include "data.h"
#include "http.h"
#include "sensor.h"
#include "wifi.h"
#include "config.h"
#include "buffer.h"
#include "setup.h"
#include "utils.h"
#include "time_utils.h"
#include "mqtt.h"
#include "ha.h"
#include "ina226.h"

#define INA226_MODE

#ifndef ESP8266
  #define ESP8266
#endif

#define BUTTON 15
#define BTN_HOLD_SETUP 5000
#define BTN_CLICK 200

//#define OTA_DISABLE
#ifndef OTA_DISABLE
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

#define RTC_ENABLE
#ifdef RTC_ENABLE
#include "rtc_utils.h"
#endif

extern "C" {
#include "user_interface.h"
extern struct rst_info resetInfo;
}

Data data;
DynamicJsonDocument json_data(JSON_BUFFER);
uint8_t attempt = 0;
#ifndef INA226_MODE
uint32_t raw = 0;
#else
float raw = 100.0f;
#define I2C_ADDRESS 0x40
INA226 ina226(I2C_ADDRESS);
#endif
double voltage = 0.0;
uint64_t sleep_period = DEFAULT_SLEEP_PERIOD * 60e6;

String resetReason;
bool sleep = true;
bool taboo = false;

EEPROMBuff<BoardConfig> storage(8);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

bool updateConfig(String &topic, String &payload) {
  bool updated = false;
  int period = 0;
  int interval = 0;
  double coeff = DEFAULT_COEFF;

  if (topic.endsWith(F("/set"))) {
    int endslash = topic.lastIndexOf('/');
    int prevslash = topic.lastIndexOf('/', endslash - 1);
    String param = topic.substring(prevslash + 1, endslash);

    rlog_i("info", "MQTT CALLBACK: Parameter: %s", param.c_str());
  
    if (param.equals(F("send_period"))) {
      period = payload.toInt();
      if (period > 0) {
        if (period != data.conf.sleep_period) {
          data.conf.sleep_period = period;
          sleep_period = period * 60e6;
          if (json_data.containsKey("send_period")) {
            json_data[F("send_period")] = period;
          }
          updated = true;
          rlog_i("info", "MQTT CALLBACK: new value of sleep_period: %d",  period);
        }
      }
    }
    if (param.equals(F("coeff"))) {
      coeff = payload.toDouble();
      if (coeff != data.conf.coeff) {
        data.conf.coeff = coeff;
        if (json_data.containsKey("coeff")) {
          json_data[F("coeff")] = coeff;
        }
        updated = true;
        rlog_i("info", "MQTT CALLBACK: new value of coeff: %f", coeff);
      }
    }
    if (param.equals(F("interval"))) {
      interval = payload.toInt();
      if (interval != data.conf.interval) {
        data.conf.interval = interval;
        if (json_data.containsKey("count")) {
          json_data[F("count")] = interval;
        }
        updated = true;
        rlog_i("info", "MQTT CALLBACK: new value of interval: %d", interval);
      }
    }
  }
  return updated;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String mTopic = topic;
  String mPayload;
  bool updated = false;

  mPayload.reserve(length);

  for (unsigned int i = 0; i < length; i++) {
    mPayload += (char)payload[i];
  }
  updated = updateConfig(mTopic, mPayload);
  mPayload.clear();
  if (updated) {
    storeConfig(data.conf);
    String topic = data.conf.mqtt_topic;
    removeSlash(topic);
    publishData(mqttClient, topic, json_data, data.conf.mqtt_auto_discovery);
  }
}

bool reconnect() {
  if (!isMQTT(data.conf)) {
    return false;
  }
  String client_id = getDeviceName();
  String topic = data.conf.mqtt_topic;
  removeSlash(topic);
  String subscribe_topic = topic + F(MQTT_ALL_TOPICS);
  int attempts = MQTT_MAX_TRIES;
  const char *login = data.conf.mqtt_login[0] ? data.conf.mqtt_login : NULL;
  const char *pass = data.conf.mqtt_password[0] ? data.conf.mqtt_password : NULL;

  while (!mqttClient.connected() && attempts--) {
    if (mqttClient.connect(client_id.c_str(), login, pass)) {
      mqttClient.subscribe(subscribe_topic.c_str(), MQTT_QOS);
      return true;
    } else {
      delay(MQTT_CONNECT_DELAY);
    }
  }
  return mqttClient.connected();
}

void setupBoard() {
  
  digitalWrite(BOARD_LED, LOW);
  delay(10000);

  WiFi.persistent(false);
  WiFi.disconnect();
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  wifiSetMode(WIFI_AP_STA);
  delay(1000);

  startAP(data.conf);

  wifiShutdown();
  
  rlog_i("info", "Restart ESP");
  ESP.restart();
}

float getVoltage226() {
  
  Wire.begin();
  uint8_t rc = 16;
  if (!ina226.begin()) {
    rlog_i("info", "INA226 init fail.");
    while (rc--) delay(5);
  }
  return ina226.getBusVoltage();
}

void setup() {

#ifdef DEBUG_INFO
  Serial.begin(115200);
  delay(1000);
  Serial.println();
#endif
  pinMode(BOARD_LED, OUTPUT);

  rlog_i("info", "Boot ok");

  bool success = loadConfig(data.conf);
  rlog_i("info", "loadConfig = %d", success);
  
  // rlog_i("info", "version = %d", data.conf.version);
  // rlog_i("info", "ssid = %s", data.conf.ssid); 
  // rlog_i("info", "password = %s", data.conf.password);
  // rlog_i("info", "sleep_period = %d", data.conf.sleep_period);
  // rlog_i("info", "mqtt_host = %s", data.conf.mqtt_host);
  // rlog_i("info", "mqtt_port = %d", data.conf.mqtt_port);
  // rlog_i("info", "mqtt_login = %s", data.conf.mqtt_login);
  // rlog_i("info", "mqtt_password = %s", data.conf.mqtt_password);
  // rlog_i("info", "mqtt_topic = %s", data.conf.mqtt_topic);
  // rlog_i("info", "mqtt_discovery_topic = %s", data.conf.mqtt_discovery_topic);
  // rlog_i("info", "mqtt_auto_discovery = %d", data.conf.mqtt_auto_discovery);
  // rlog_i("info", "stat_host = %s", data.conf.stat_host);
  // rlog_i("info", "ip = %d", data.conf.ip);
  // rlog_i("info", "gateway = %d", data.conf.gateway);
  // rlog_i("info", "mask = %d", data.conf.mask);
  // rlog_i("info", "ntp_server = %s", data.conf.ntp_server);
  // rlog_i("info", "tz = %d", data.conf.tz);
  // rlog_i("info", "wifi_bssid = %d", data.conf.wifi_bssid);
  // rlog_i("info", "wifi_channel = %d", data.conf.wifi_channel);
  // rlog_i("info", "wifi_phy_mode = %d", data.conf.wifi_phy_mode);

  sleep_period = data.conf.sleep_period * 60e6;
  rlog_i("info", "period = %lld", sleep_period);

  if (!success || !data.conf.sleep_period) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }
  
  switch(resetInfo.reason) {
    // normal startup by power on
    case REASON_DEFAULT_RST:      //{sleep = true; break;}
    // hardware watch dog reset
    case REASON_WDT_RST:          //{sleep = true; break;}
    // exception reset, GPIO status won’t change
    case REASON_EXCEPTION_RST:    //{sleep = true; break;}
    // software watch dog reset, GPIO status won’t change
    case REASON_SOFT_WDT_RST:     //{sleep = true; break;}
    // software restart ,system_restart , GPIO status won’t change
    case REASON_SOFT_RESTART:     //{sleep = true; break;}
    // wake up from deep-sleep
    case REASON_DEEP_SLEEP_AWAKE: {sleep = true; break;}
    // external system reset
    case REASON_EXT_SYS_RST:      {sleep = false; taboo = true; break;}
    default:                      {sleep = true; break;}
  }
  
  resetReason = ESP.getResetReason();
  rlog_i("info", "Reset reason: >%s< to sleep = %d taboo = %d info.reason=%d", resetReason.c_str(), sleep, taboo, resetInfo.reason);

  flashLED();

  uint8_t count = data.conf.interval;
  if(count > 16) count = 16;
#ifdef RTC_ENABLE
  struct Avg {
    double raw[16] = {0.0};
  };
  uint8_t rc;
  Avg avg;

  rc = rtc_read(&avg);
  rlog_i("info", "RTC read: rc = %d avg0 = %f avg1 = %f avg2 = %f", rc, avg.raw[0], avg.raw[1], avg.raw[2]);
  // if(rc != 1) {
  // }
#endif  

// get voltage
#ifndef INA226_MODE
  pinMode(A0, OUTPUT);
  raw = analogRead(A0);
#else
  // Wire.begin();
  // rc = 16;
  // if (!ina226.begin()) {
  //   rlog_i("info", "INA226 init fail.");
  //   while (rc--) delay(5);
  // }
  // raw = ina226.getBusVoltage();
  int att = 16;
  while(att-- && raw > 50.0f) {
    delay(10);
    raw = getVoltage226();
    rlog_i("info", "INA226 get fail. raw=%f att=%d", raw, att);
  }
  if(att < 0) {
    raw = 0.0f;
    rlog_i("info", "INA226 exit. raw=%f att=%d", raw, att);
  }
#endif

#ifdef RTC_ENABLE
  double total = (double)raw;
  rc = 16;
  for(int i=15; i>0; i--) {
    avg.raw[i] = avg.raw[i-1];
    if(avg.raw[i] == 0.0 || i >= count) {
      rc--;
    } else {
      total += avg.raw[i];
    }
  }
  avg.raw[0] = (double)raw;
  #ifndef INA226_MODE
    voltage = total / 1024.0 / (double)rc;
  #else
    voltage = total / (double)rc;
  #endif
  rlog_i("info", "average: interval = %d coeff=%f", rc, data.conf.coeff);
  rc = rtc_write(&avg);
  rlog_i("info", "RTC write: rc = %d avg0 = %f avg1 = %f avg2 = %f voltage = %f", rc, avg.raw[0], avg.raw[1], avg.raw[2], voltage);
#else  
  #ifndef INA226_MODE
  voltage = raw / 1024.0;
  #else
  voltage = raw;
  #endif
#endif  
  
  voltage = voltage * data.conf.coeff;
  long mv  = voltage * 10000.0 + 0.5;
  voltage = mv / 10000.0;
  data.data.voltage = voltage;
  rlog_i("info", "average: mv = %d voltage = %f", mv, voltage);

// try to connect
  success = wifiConnect(data.conf);
  rlog_i("info", "WiFi connect = %d", success);

  if (!success) {
    rlog_i("info", "Setup board entering");
    setupBoard();
  }

  if (success && isNTP(data.conf)) {
    success = syncTime(data.conf);
    rlog_i("info", "sync_ntp_time = %d", success);
  }

  if (isMQTT(data.conf) && (WiFi.status() == WL_CONNECTED)) {
    rlog_i("info", "MQTT begin");
    espClient.setTimeout(MQTT_SOCKET_TIMEOUT * 1000);
    mqttClient.setBufferSize(MQTT_MAX_PACKET_SIZE);
    mqttClient.setServer(data.conf.mqtt_host, data.conf.mqtt_port);
    mqttClient.setSocketTimeout(MQTT_SOCKET_TIMEOUT);
    mqttClient.setCallback(mqttCallback);
  }

  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(BOARD_LED, LOW);
    getTempC(data.data);
    getJSONData(data, json_data);
    
    if (isStat(data.conf)) {
      sendHTTP(data.conf.stat_host, json_data);
    }
    
    if (isMQTT(data.conf)) {
      if (reconnect()) {
        String topic = data.conf.mqtt_topic;
        removeSlash(topic);
        publishData(mqttClient, topic, json_data, data.conf.mqtt_auto_discovery);
        String discovery_topic = data.conf.mqtt_discovery_topic;
        publishHA(mqttClient, topic, discovery_topic);

        mqttClient.loop();
        sleep = false;
      }
    }
  
  #ifndef OTA_DISABLE
    ArduinoOTA.begin();
  #endif
  } else if (WiFi.status() == WL_WRONG_PASSWORD) {
    setupBoard();
  }

  digitalWrite(BOARD_LED, HIGH);
}

bool flag = false;
uint32_t btnTimer = 0;
uint32_t waitTimer = 0;
uint32_t secTimer = 0;
uint32_t mqttTimer = 0;
bool skip = true;

void loop() {
  
  mqttClient.loop();

  if (sleep) {
    delay (1000);
    rlog_i("info", "System is going to sleep...");
    ESP.deepSleep (sleep_period);
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
    rlog_i("info loop >>>>>", "MAKE CLICK and RESTART");
    ESP.restart();
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
  
  if (millis() - mqttTimer >= 1 * PERIOD_SEC) {
    mqttTimer = millis();
    if (!sleep && !skip && !taboo) {
      sleep = true;
      rlog_i("info loop >>>>>", "sleep is true!");
    }
    skip = !skip;
  }

  if (millis() - secTimer >= 1 * PERIOD_SEC) {
    secTimer = millis();
    flashLED();
  }
}
