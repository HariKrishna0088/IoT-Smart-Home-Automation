<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge" alt="MQTT"/>
  <img src="https://img.shields.io/badge/Framework-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Category-IoT-green?style=for-the-badge" alt="IoT"/>
</p>

# 🏠 IoT Smart Home Automation System

> A complete ESP32-based home automation system with MQTT cloud communication, built-in web dashboard, multi-sensor monitoring, and intelligent auto-control logic.

---

## 🔍 Overview

This project implements a **full-stack IoT home automation system** using the ESP32 microcontroller. It monitors environmental conditions (temperature, humidity, light, motion) and controls home appliances via 4-channel relay module — all accessible through a beautiful **local web dashboard** and **MQTT cloud interface**.

### Key Highlights
- 🌡️ **Multi-Sensor Monitoring** — DHT22, LDR, PIR
- ⚡ **4-Channel Relay Control** — Light, Fan, AC, Spare
- 📡 **MQTT Cloud Communication** — Publish/Subscribe with JSON
- 🌐 **Built-in Web Dashboard** — Glassmorphism UI with real-time updates
- 🤖 **Auto-Control Logic** — Smart fan/light based on conditions
- 🔄 **OTA Updates** — Over-the-Air firmware updates
- 📱 **Mobile Responsive** — Dashboard works on any device

---

## 🏗️ System Architecture

```
┌─────────────┐     ┌──────────────┐     ┌─────────────────┐
│  Sensors    │     │   ESP32      │     │   MQTT Cloud    │
│  ┌───────┐  │     │              │     │   (HiveMQ/AWS)  │
│  │ DHT22 ├──┼────►│  ┌────────┐  │◄───►│                 │
│  │ LDR   ├──┤     │  │ Control │  │     │  ┌───────────┐  │
│  │ PIR   ├──┤     │  │ Logic   │  │     │  │Dashboard  │  │
│  └───────┘  │     │  └────┬───┘  │     │  │(Remote)   │  │
│             │     │       │      │     │  └───────────┘  │
│  ┌───────┐  │     │  ┌────▼───┐  │     └─────────────────┘
│  │Relays ├──┼────►│  │Web     │  │
│  │ (x4)  │  │     │  │Server  │  │◄──── 📱 Local Dashboard
│  └───────┘  │     │  └────────┘  │      http://[ESP32-IP]
└─────────────┘     └──────────────┘
```

---

## 🔌 Circuit Connections

| Component | ESP32 Pin | Description |
|:---------:|:---------:|:------------|
| DHT22 Data | GPIO 4 | Temperature & Humidity |
| LDR | GPIO 34 (ADC) | Ambient Light Level |
| PIR | GPIO 27 | Motion Detection |
| Relay CH1 | GPIO 16 | 💡 Light Control |
| Relay CH2 | GPIO 17 | 🌀 Fan Control |
| Relay CH3 | GPIO 18 | ❄️ AC Control |
| Relay CH4 | GPIO 19 | 🔌 Spare |
| Buzzer | GPIO 26 | Alert Buzzer |
| Status LED | GPIO 2 | WiFi Status |

---

## 📁 File Structure

```
IoT-Smart-Home-Automation/
├── src/
│   └── main.cpp            # Main application code
├── data/
│   └── (SPIFFS web files)
├── docs/
│   └── circuit_diagram.md
├── platformio.ini          # PlatformIO configuration
├── .gitignore
├── LICENSE
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites
- ESP32 Development Board
- DHT22 Sensor, LDR, PIR Sensor
- 4-Channel Relay Module
- PlatformIO IDE (VS Code Extension)

### Setup
```bash
# Clone the repository
git clone https://github.com/Harikrishna_08/IoT-Smart-Home-Automation.git
cd IoT-Smart-Home-Automation

# Edit WiFi credentials in src/main.cpp
# WIFI_SSID and WIFI_PASSWORD

# Build & Upload
pio run --target upload

# Open Serial Monitor
pio device monitor
```

---

## 🤖 Auto-Control Logic

| Condition | Action |
|:----------|:-------|
| Dark + Motion detected | 💡 Light turns ON |
| Temperature > 30°C | 🌀 Fan turns ON |
| Temperature < 26°C | 🌀 Fan turns OFF |

---

## 👨‍💻 Author

**Daggolu Hari Krishna** — B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://linkedin.com/in/harikrishnadaggolu)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">⭐ Star this repo if you found it useful! ⭐</p>
