// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "mqtt.h"

void publish_chunked(PubSubClient &mqtt_client, String &topic, String &payload, unsigned int chunk_size) {

  unsigned int len = payload.length();
  const uint8_t *buf = (const uint8_t *)payload.c_str();

  if (mqtt_client.beginPublish(topic.c_str(), len, true)) {
    while (len > 0) {
      if (len >= chunk_size) {
        mqtt_client.write(buf, chunk_size);
        buf += chunk_size;
        len -= chunk_size;
      } else {
        mqtt_client.write(buf, len);
        break;
      }
    }
    mqtt_client.endPublish();
  }  
}

void publish_big(PubSubClient &mqtt_client, String &topic, String &payload) {

  unsigned int len = payload.length();
  if (mqtt_client.beginPublish(topic.c_str(), len, true)) {
    mqtt_client.print(payload.c_str());
    mqtt_client.endPublish();
  }
}

void publish_simple(PubSubClient &mqtt_client, String &topic, String &payload) {

  if (mqtt_client.connected()) {
    mqtt_client.publish(topic.c_str(), payload.c_str(), true);
  }
}

void publish(PubSubClient &mqtt_client, String &topic, String &payload, int mode) {
  switch (mode) {
    case PUBLISH_MODE_SIMPLE:
      publish_simple(mqtt_client, topic, payload);
      break;
    case PUBLISH_MODE_CHUNKED:
      publish_chunked(mqtt_client, topic, payload, MQTT_CHUNK_SIZE);
      break;
    case PUBLISH_MODE_BIG:
    default:
      publish_big(mqtt_client, topic, payload);
  }
}

void publishToSingleTopic(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data) {
  String payload = "";
  serializeJson(json_data, payload);
  publish(mqtt_client, topic, payload, DEFAULT_PUBLISH_MODE);
}

void publishToMultipleTopics(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data) {
  JsonObject root = json_data.as<JsonObject>();
  for (JsonPair p : root) {
    String sensor_topic = topic + "/" + p.key().c_str();
    String sensor_value = p.value().as<String>();
    publish(mqtt_client, sensor_topic, sensor_value, DEFAULT_PUBLISH_MODE);
  }
}

void publishData(PubSubClient &mqtt_client, String &topic, DynamicJsonDocument &json_data, bool auto_discovery) {
  unsigned long start = millis();

  if (auto_discovery) {
    publishToSingleTopic(mqtt_client, topic, json_data);
  } else {
    publishToMultipleTopics(mqtt_client, topic, json_data);
  }
}
