# Smart Goggles (Aurora)

A wearable ESP32-based smart goggles prototype combining GPS location tracking, a live sensor readout, WiFi connectivity, and fingerprint-based authentication. Self-initiated project.

> **Note:** Photos of the physical build are not available. Concept artwork used during design is included separately and labeled as a concept illustration, not a photo of the finished device.

## Features

- **Fingerprint-based authentication:** the device stays locked until a fingerprint is verified using the Adafruit Fingerprint sensor library. Only after a successful match does the main interface become active.
- **GPS tracking:** reads live latitude, longitude, and time from a GPS module using TinyGPSPlus over a hardware serial connection.
- **OLED display:** shows time, GPS coordinates, WiFi status, and sensor data on a 128x64 SSD1306 display.
- **WiFi connectivity:** connects to a WiFi network on startup and displays connection status and IP address.
- **Motion sensing:** reads acceleration data from an MPU6050 (accelerometer + gyroscope).
- **Touch control:** a touch sensor toggles between the main information screen and a live sensor data screen.
- **Battery level reading:** reads battery voltage via an analog pin.

## Hardware

| Component | Role |
|---|---|
| ESP32 dev board | Main controller for all logic and sensors |
| GPS module (serial, e.g. NEO-6M-class) | Location and time data |
| SSD1306 OLED (128x64, I2C) | On-goggle display |
| Fingerprint sensor (Adafruit Fingerprint-compatible, e.g. R307/AS608) | User authentication |
| MPU6050 | Accelerometer + gyroscope motion data |
| Touch sensor | Screen switching |
| Battery + voltage divider | Battery level monitoring |

## Pin connections (from the code)

| Signal | ESP32 Pin |
|---|---|
| I2C SDA / SCL (OLED, MPU6050) | GPIO21 / GPIO22 |
| GPS RX / TX | GPIO16 / GPIO17 |
| Fingerprint sensor RX / TX | GPIO26 / GPIO27 |
| Touch sensor | GPIO4 |
| Battery sense | GPIO34 |

## How it works

1. On startup, the ESP32 initializes the OLED, GPS serial, fingerprint sensor, MPU6050, and connects to WiFi.
2. The device shows "Place finger..." and waits for a fingerprint match before unlocking.
3. Once authenticated, GPS data is continuously read in the background.
4. The touch sensor toggles between:
   - **Main screen:** time, GPS coordinates, and WiFi status.
   - **Sensor screen:** live accelerometer readings and battery level.

## Arduino libraries used

- Adafruit Fingerprint Sensor Library
- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit MPU6050
- Adafruit Unified Sensor
- TinyGPSPlus

## Repository contents

- `smart_goggles.ino`: main ESP32 sketch (fingerprint auth, GPS, OLED, WiFi, touch, MPU6050)
- `concept_illustration.jpg`: early concept artwork (not a photo of the built device)

## Setup

1. Wire the components as listed in the pin table above.
2. Install the libraries listed above via the Arduino Library Manager.
3. Replace `YOUR_WIFI_NAME` and `YOUR_WIFI_PASSWORD` in the code with your own network credentials before uploading. Do not commit real credentials to this repository.
4. Enroll at least one fingerprint using the Adafruit Fingerprint library's enrollment example before running the main sketch.
5. Upload to the ESP32 using the Arduino IDE.

## What I learned

I hadn't worked with multiple hardware serial ports on one board before, so getting the GPS module and fingerprint sensor to run at the same time without interfering with each other took some trial and error. I also had to think through how to structure the code so the device stays locked and does nothing else until the fingerprint check passes, which was a good exercise in writing authentication logic rather than just reading sensors.

## Possible improvements

Right now the goggles only support one enrolled fingerprint. A next step would be storing multiple users, and adding a low-power sleep mode so the battery lasts longer when the device is idle.

## Author

Nikhil Gaurav, B.Tech ECE, Andhra University
