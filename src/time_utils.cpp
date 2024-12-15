// Copyright 2024 vals. All rights reserved.
// SPDX-License-Identifier: MIT

#include "time_utils.h"

bool syncTime(BoardConfig &conf) {
  
  configTime(conf.tz*3600, 0, conf.ntp_server);
  
  int attempts = 0;
  time_t now = time(nullptr);
  while(now < START_VALID_TIME) {
    yield();
    now = time(nullptr);
    attempts++;
    if (attempts > NTP_ATTEMPTS) {
      return false;
    }
    delay(500);
  };
  return true;
}

String getLocalTime() {
  
  char buf[100];
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  strftime(buf, sizeof(buf), TIME_FORMAT, &timeinfo);
  return String(buf);
}

void getUpTime(uint32_t &start, char* buf) {
  
  uint32_t uptime = time(nullptr);
  uint32_t sec = uptime - start;
  uint32_t min = sec / 60UL;
  uint32_t hour = min / 60UL;
  uint32_t day = hour / 24UL;
  uint8_t ss = sec % 60;
  uint8_t mi = min % 60;
  uint8_t hh = hour % 24;

  sprintf(buf,"%d days %02d:%02d:%02d", day, hh, mi, ss);
}
