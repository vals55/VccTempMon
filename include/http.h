// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "json.h"
#include "rlog.h"

#define HTTP_SEND_ATTEMPTS 3
#define SERVER_TIMEOUT 10000
#define PROTO_HTTPS "https"
#define PROTO_HTTP "http"

String getProtocol(const String &url) {
	String proto = PROTO_HTTP;
	int index = url.indexOf(':');
	if (index > 0) {
		proto = url.substring(0, index);
		proto.toLowerCase();
	}
	return proto;
}

bool postData(const String &url, String &payload) {
  void *pClient = nullptr;
  HTTPClient httpClient;
  bool result = false;
  
  String proto = getProtocol(url);
  if (proto == PROTO_HTTP) {
    pClient = new WiFiClient;
  } else if (proto == PROTO_HTTPS) {
    pClient = new WiFiClientSecure;
    (*(WiFiClientSecure *)pClient).setInsecure();
  }

  httpClient.setTimeout(SERVER_TIMEOUT);
  httpClient.setReuse(false);

  if (httpClient.begin(*(WiFiClient *)pClient, url)) {
    httpClient.addHeader("Content-Type", "application/json");

    int response_code = httpClient.POST(payload);
    result = response_code == 200;
    String response_body = httpClient.getString();
    httpClient.end();
    (*(WiFiClient *)pClient).stop();
  }
  delete (WiFiClient*)pClient;
  return result;
}

bool sendHTTP(const String &url, const DynamicJsonDocument &json_data) {
  uint32_t start_time = millis();
  String payload = "";

  serializeJson(json_data, payload);

  int attempts = HTTP_SEND_ATTEMPTS;
  bool result = false;
  do {
    result = postData(url, payload);
  } while (!result && --attempts);

  // if (result) {
  //   rlog_i("info", "HTTP: Data sent. Time %d ms", millis() - start_time);
  // } else {
  //    rlog_i("info", "HTTP: Failed send. Time %d ms", millis() - start_time);
  // }
  return result;
}

