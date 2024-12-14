// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include "resources.h"
#include "mqtt.h"
#include "config.h"
#include "json.h"

#define MQTT_FORCE_UPDATE true

void publishHA(PubSubClient &mqtt_client, String &topic, String &discovery_topic);
