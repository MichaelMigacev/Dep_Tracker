#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"
#include "config.h"
#include "Wifi_Connection.h"

WiFiConnection wifiConn;

static constexpr uint8_t DEBOUNCE_DELAY_MS = 20U;

void setup()
{
  Serial.begin(115200);

  wifiConn.setupWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts);

  // init and get time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  wifiConn.maintainWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts,
      checkInterval);
}
