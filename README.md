# MQTT Temp Sensor

A simple Wi-Fi temperature and humidity sensor using MQTT for data reporting. Designed for easy deployment around an office for environment monitoring and automation.

## Hardware
- **Board:** Waveshare ESP32-C3-Zero  
- **Sensor:** SHT31 (I²C)

## Software
- **Framework:** Arduino IDE
- **MQTT Library:** PubSubClient  
- **Broker:** HiveMQ (TLS-enabled)

## Features
- Samples temperature and humidity every 5 minutes  
- Publishes data to MQTT topic   
- Wi-Fi credentials and MQTT settings stored in a separate file (`secrets.h`)  
- Can integrate with **n8n**, **Home Assistant**, or other automation tools  
