<p align="center">
  <img src="https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32"/>
  <img src="https://img.shields.io/badge/Protocol-MQTT-orange?style=for-the-badge" alt="MQTT"/>
  <img src="https://img.shields.io/badge/Framework-Arduino-teal?style=for-the-badge&logo=arduino&logoColor=white" alt="Arduino"/>
  <img src="https://img.shields.io/badge/Category-IoT-green?style=for-the-badge" alt="IoT"/>
</p>

# ðŸ  IoT Smart Home Automation System

> A complete ESP32-based home automation system with MQTT cloud communication, built-in web dashboard, multi-sensor monitoring, and intelligent auto-control logic.

---

## ðŸ” Overview

This project implements a **full-stack IoT home automation system** using the ESP32 microcontroller. It monitors environmental conditions (temperature, humidity, light, motion) and controls home appliances via 4-channel relay module â€” all accessible through a beautiful **local web dashboard** and **MQTT cloud interface**.

### Key Highlights
- ðŸŒ¡ï¸ **Multi-Sensor Monitoring** â€” DHT22, LDR, PIR
- âš¡ **4-Channel Relay Control** â€” Light, Fan, AC, Spare
- ðŸ“¡ **MQTT Cloud Communication** â€” Publish/Subscribe with JSON
- ðŸŒ **Built-in Web Dashboard** â€” Glassmorphism UI with real-time updates
- ðŸ¤– **Auto-Control Logic** â€” Smart fan/light based on conditions
- ðŸ”„ **OTA Updates** â€” Over-the-Air firmware updates
- ðŸ“± **Mobile Responsive** â€” Dashboard works on any device

---

## ðŸ—ï¸ System Architecture

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚  Sensors    â”‚     â”‚   ESP32      â”‚     â”‚   MQTT Cloud    â”‚
â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”  â”‚     â”‚              â”‚     â”‚   (HiveMQ/AWS)  â”‚
â”‚  â”‚ DHT22 â”œâ”€â”€â”¼â”€â”€â”€â”€â–ºâ”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚â—„â”€â”€â”€â–ºâ”‚                 â”‚
â”‚  â”‚ LDR   â”œâ”€â”€â”¤     â”‚  â”‚ Control â”‚  â”‚     â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”  â”‚
â”‚  â”‚ PIR   â”œâ”€â”€â”¤     â”‚  â”‚ Logic   â”‚  â”‚     â”‚  â”‚Dashboard  â”‚  â”‚
â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚     â”‚  â””â”€â”€â”€â”€â”¬â”€â”€â”€â”˜  â”‚     â”‚  â”‚(Remote)   â”‚  â”‚
â”‚             â”‚     â”‚       â”‚      â”‚     â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚
â”‚  â”Œâ”€â”€â”€â”€â”€â”€â”€â”  â”‚     â”‚  â”Œâ”€â”€â”€â”€â–¼â”€â”€â”€â”  â”‚     â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
â”‚  â”‚Relays â”œâ”€â”€â”¼â”€â”€â”€â”€â–ºâ”‚  â”‚Web     â”‚  â”‚
â”‚  â”‚ (x4)  â”‚  â”‚     â”‚  â”‚Server  â”‚  â”‚â—„â”€â”€â”€â”€ ðŸ“± Local Dashboard
â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚     â”‚  â””â”€â”€â”€â”€â”€â”€â”€â”€â”˜  â”‚      http://[ESP32-IP]
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜     â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

---

## ðŸ”Œ Circuit Connections

| Component | ESP32 Pin | Description |
|:---------:|:---------:|:------------|
| DHT22 Data | GPIO 4 | Temperature & Humidity |
| LDR | GPIO 34 (ADC) | Ambient Light Level |
| PIR | GPIO 27 | Motion Detection |
| Relay CH1 | GPIO 16 | ðŸ’¡ Light Control |
| Relay CH2 | GPIO 17 | ðŸŒ€ Fan Control |
| Relay CH3 | GPIO 18 | â„ï¸ AC Control |
| Relay CH4 | GPIO 19 | ðŸ”Œ Spare |
| Buzzer | GPIO 26 | Alert Buzzer |
| Status LED | GPIO 2 | WiFi Status |

---

## ðŸ“ File Structure

```
IoT-Smart-Home-Automation/
â”œâ”€â”€ src/
â”‚   â””â”€â”€ main.cpp            # Main application code
â”œâ”€â”€ data/
â”‚   â””â”€â”€ (SPIFFS web files)
â”œâ”€â”€ docs/
â”‚   â””â”€â”€ circuit_diagram.md
â”œâ”€â”€ platformio.ini          # PlatformIO configuration
â”œâ”€â”€ .gitignore
â”œâ”€â”€ LICENSE
â””â”€â”€ README.md
```

---

## ðŸš€ Getting Started

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

## ðŸ¤– Auto-Control Logic

| Condition | Action |
|:----------|:-------|
| Dark + Motion detected | ðŸ’¡ Light turns ON |
| Temperature > 30Â°C | ðŸŒ€ Fan turns ON |
| Temperature < 26Â°C | ðŸŒ€ Fan turns OFF |

---

## ðŸ‘¨â€ðŸ’» Author

**Daggolu Hari Krishna** â€” B.Tech ECE | JNTUA College of Engineering, Kalikiri

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue?style=flat-square&logo=linkedin)](https://www.linkedin.com/in/contacthari88/)
[![GitHub](https://img.shields.io/badge/GitHub-Harikrishna__08-black?style=flat-square&logo=github)](https://github.com/Harikrishna_08)

---

<p align="center">â­ Star this repo if you found it useful! â­</p>
