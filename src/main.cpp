#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include "config.h"
#include "Wifi_Connection.h"
#include "Station_Info.h"

WiFiConnection wifiConn;
StationInfo statInf;

// Train monitoring
const uint32_t trainsUpdateInterval = 20000UL; // 20 seconds
uint32_t trainLastCheck = 0UL;
uint32_t previousMillis = 0UL;
uint32_t currentMillis = 0UL;

const uint8_t LEVER_PIN = 15U;
bool lastLeverState = HIGH;
bool currentLeverState = HIGH;

String myStartStation = fromStation;
String myEndStation = toStation;

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

  pinMode(LEVER_PIN, INPUT_PULLUP); // Internal pull-up

  currentLeverState = digitalRead(LEVER_PIN);
  if (currentLeverState == LOW)
  {
    myStartStation = fromStationTwo;
    myEndStation = toStationTwo;
  }
  else
  {
    myStartStation = fromStation;
    myEndStation = toStation;
  }
  previousMillis = millis() - trainsUpdateInterval; // Force immediate update
}

void loop()
{
  wifiConn.maintainWiFi(
      ssid,
      password,
      connectionAttempts,
      maxAttempts,
      checkInterval);

  currentMillis = millis();

  if (currentMillis - previousMillis >= trainsUpdateInterval)
  {
    previousMillis = currentMillis;

    // Get departures
    DepartureList departures = statInf.getSouthboundJourneys(myStartStation, myEndStation);
  }

  bool reading = digitalRead(LEVER_PIN);

  if (reading != lastLeverState)
  {
    // Small debounce delay
    delay(DEBOUNCE_DELAY_MS);

    reading = digitalRead(LEVER_PIN); // Read again

    if (reading != currentLeverState)
    {
      currentLeverState = reading;
      if (currentLeverState == LOW)
      {
        myStartStation = fromStationTwo;
        myEndStation = toStationTwo;
      }
      else
      {
        myStartStation = fromStation;
        myEndStation = toStation;
      }
      DepartureList departures = statInf.getSouthboundJourneys(myStartStation, myEndStation);
    }
  }
  lastLeverState = reading;
}
