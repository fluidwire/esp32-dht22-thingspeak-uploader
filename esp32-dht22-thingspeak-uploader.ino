/*
 * ESP32 DHT22 ThingSpeak Uploader
 * -------------------------------
 * Reads a DHT22 on GPIO4 and writes temperature, humidity and heat index to
 * a ThingSpeak channel (fields 1-3). A staple for student weather stations.
 *
 * Notes:
 *   - ThingSpeak's free tier accepts an update at most once every 15 s.
 *     This sketch uses 20 s to stay comfortably inside the limit.
 *   - Sensor reads are NaN-safe: a failed read skips the upload instead of
 *     sending garbage to your channel.
 *   - HTTP status codes from ThingSpeak are decoded into readable messages.
 *
 * Required libraries (install via Library Manager):
 *   - "DHT sensor library" by Adafruit
 *   - "Adafruit Unified Sensor" by Adafruit
 *   - "ThingSpeak" by MathWorks
 *
 * Board: ESP32 Dev Module (any ESP32 dev board).
 *
 * Wiring (DHT22 -> ESP32):
 *   DHT22 VCC  -> 3V3
 *   DHT22 GND  -> GND
 *   DHT22 DATA -> GPIO4 with a 10k pull-up to 3V3
 *
 * Setup:
 *   1. Create a ThingSpeak channel with 3 fields (temp, humidity, heat index).
 *   2. Copy config.h.example to config.h; fill WiFi + channel ID + write key.
 *   3. Flash. Open Serial Monitor at 115200.
 *
 * (c) 2026 Fluidwire. Released under the MIT License.
 * https://github.com/fluidwire
 */

#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include "config.h"

#define DHT_PIN  4
#define DHT_TYPE DHT22

const uint32_t UPLOAD_INTERVAL_MS = 20000;   // >= 15 s for free tier

WiFiClient wifiClient;
DHT        dht(DHT_PIN, DHT_TYPE);
uint32_t   lastUpload = 0;

void connectWiFi() {
  Serial.printf("WiFi: connecting to %s ", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf(" OK, ip=%s\n", WiFi.localIP().toString().c_str());
}

const char *describeStatus(int code) {
  switch (code) {
    case 200:  return "OK";
    case 404:  return "channel ID or write key not found";
    case -401: return "rate limited (update faster than 15 s)";
    case -301: return "could not connect to ThingSpeak";
    case -302: return "unexpected failure during write";
    case 0:    return "no response / network problem";
    default:   return "unexpected status";
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  dht.begin();
  connectWiFi();
  ThingSpeak.begin(wifiClient);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();

  uint32_t now = millis();
  if (now - lastUpload < UPLOAD_INTERVAL_MS) return;
  lastUpload = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("DHT read failed, skipping upload");
    return;
  }
  float hi = dht.computeHeatIndex(t, h, false);   // Celsius

  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, hi);

  int code = ThingSpeak.writeFields(TS_CHANNEL_ID, TS_WRITE_API_KEY);
  if (code == 200) {
    Serial.printf("ThingSpeak OK: temp=%.1fC humidity=%.1f%% heat_index=%.1fC\n", t, h, hi);
  } else {
    Serial.printf("ThingSpeak write failed, HTTP %d (%s)\n", code, describeStatus(code));
  }
}
