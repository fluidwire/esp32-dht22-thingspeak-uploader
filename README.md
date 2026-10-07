# ESP32-DHT22-THINGSPEAK-UPLOADER — upload DHT22 readings to ThingSpeak fields

IoT example by [Fluidwire](https://fluidwire.com/).

Reads a DHT22 on an ESP32 and uploads temperature, humidity and heat index to a ThingSpeak channel every 20 seconds. ThingSpeak gives you free, hosted time-series charts with no backend to build — a staple for student weather stations and thesis demos.

## What it does

- Connects to WiFi using credentials in `config.h`
- Every 20 s, reads the DHT22 (NaN-safe: a failed read skips the upload)
- Writes three fields (temperature, humidity, heat index) in a single `writeFields` call
- Decodes ThingSpeak status codes into readable messages

ThingSpeak's free tier accepts an update at most once every 15 seconds; the 20 s interval stays safely inside that.

## Required hardware

- 1x ESP32 dev board
- 1x DHT22 (AM2302) temperature/humidity sensor
- 1x 10 kΩ resistor (DATA-to-3V3 pull-up)
- Jumper wires, breadboard

## Required libraries

Install via the Arduino IDE Library Manager (**Sketch → Include Library → Manage Libraries…**):

- **DHT sensor library** by Adafruit
- **Adafruit Unified Sensor** by Adafruit
- **ThingSpeak** by MathWorks

You also need the ESP32 core via **Boards Manager** (`esp32` by Espressif), then pick **ESP32 Dev Module**.

## Wiring

| DHT22 pin | ESP32 pin                      |
|-----------|--------------------------------|
| VCC (1)   | 3V3                            |
| DATA (2)  | GPIO4 (with 10 kΩ pull-up)     |
| NC (3)    | not connected                  |
| GND (4)   | GND                            |

## Setup

1. At [thingspeak.com](https://thingspeak.com), create a free account and a channel with **3 fields** (Field 1 = Temperature, Field 2 = Humidity, Field 3 = Heat index).
2. Note the **Channel ID** and the **Write API Key** (channel's *API Keys* tab).
3. Copy `config.h.example` to `config.h` and fill in WiFi + channel ID + write key.
4. Select **Tools → Board → ESP32 Dev Module** and the correct COM port, then **Upload**.

## Expected output

Open **Tools → Serial Monitor**, baud rate **115200**:

```
WiFi: connecting to your-wifi-ssid .... OK, ip=192.168.1.52
ThingSpeak OK: temp=28.4C humidity=71.2% heat_index=32.6C
ThingSpeak OK: temp=28.4C humidity=71.0% heat_index=32.5C
```

## Troubleshooting

- **`HTTP -401 (rate limited)`** — updates faster than every 15 s. Keep the interval ≥ 15 s.
- **`HTTP 404`** — wrong channel ID or write key.
- **`HTTP 0` / `-301`** — no network or ThingSpeak unreachable. Confirm WiFi has an IP.
- **Chart is flat or a field is empty** — the channel has fewer than 3 fields, or field numbers don't match `setField(n, …)`.
- **`DHT read failed`** — pull-up missing, wrong pin, or sensor wired backwards. DHT22 needs ~2 s between reads; 20 s is fine.
- **WiFi never connects** — wrong SSID/password or a 5 GHz-only network. ESP32 is 2.4 GHz only.

## Security note

Don't commit `config.h` — it holds your WiFi password and ThingSpeak write key. The `.gitignore` already excludes it; only `config.h.example` is tracked. Treat the write key like a password; you can regenerate it on ThingSpeak if it leaks.

## License

MIT — see [LICENSE](LICENSE).

## Got stuck?

Drop by [fluidwire.com](https://fluidwire.com/) — we ship IoT building blocks and are happy to help.
