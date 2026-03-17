/*=============================================================================
 * IoT Smart Home Automation System
 * Author: Daggolu Hari Krishna
 * Platform: ESP32 (Arduino Framework)
 *
 * Features:
 *  - DHT22 temperature & humidity monitoring
 *  - LDR light level sensing
 *  - PIR motion detection
 *  - Relay control for appliances (4 channels)
 *  - MQTT communication with cloud (AWS IoT / Mosquitto)
 *  - Built-in web dashboard for local control
 *  - OTA (Over-the-Air) firmware updates
 *  - WiFi connectivity with auto-reconnect
 *=============================================================================*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

// ==================== Configuration ====================
// WiFi Credentials
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// MQTT Broker
const char* MQTT_BROKER   = "broker.hivemq.com"; // Public broker for demo
const int   MQTT_PORT     = 1883;
const char* MQTT_CLIENT   = "SmartHome_ESP32";
const char* MQTT_TOPIC_PUB = "smarthome/sensors";
const char* MQTT_TOPIC_SUB = "smarthome/control";

// ==================== Pin Definitions ====================
#define DHT_PIN        4      // DHT22 data pin
#define DHT_TYPE       DHT22
#define LDR_PIN        34     // LDR analog input (ADC)
#define PIR_PIN        27     // PIR motion sensor
#define RELAY_1        16     // Relay Channel 1 (Light)
#define RELAY_2        17     // Relay Channel 2 (Fan)
#define RELAY_3        18     // Relay Channel 3 (AC)
#define RELAY_4        19     // Relay Channel 4 (Spare)
#define BUZZER_PIN     26     // Alert buzzer
#define STATUS_LED     2      // Onboard LED

// ==================== Objects ====================
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
WebServer server(80);

// ==================== Global Variables ====================
float temperature = 0.0;
float humidity    = 0.0;
int   lightLevel  = 0;
bool  motionDetected = false;

bool relay1State = false;
bool relay2State = false;
bool relay3State = false;
bool relay4State = false;

unsigned long lastSensorRead = 0;
unsigned long lastMqttPublish = 0;
const long SENSOR_INTERVAL = 2000;  // 2 seconds
const long MQTT_INTERVAL   = 5000;  // 5 seconds

// ==================== Function Declarations ====================
void setupWiFi();
void setupMQTT();
void setupWebServer();
void setupOTA();
void readSensors();
void publishSensorData();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleRoot();
void handleAPI();
void handleControl();
void setRelay(int relay, bool state);
String getStatusJSON();

// ==================== Setup ====================
void setup() {
    Serial.begin(115200);
    Serial.println("\n========================================");
    Serial.println("  IoT Smart Home Automation System");
    Serial.println("  Author: Daggolu Hari Krishna");
    Serial.println("========================================\n");

    // Initialize pins
    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    pinMode(RELAY_4, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);
    pinMode(PIR_PIN, INPUT);

    // Relays OFF initially (active LOW)
    digitalWrite(RELAY_1, HIGH);
    digitalWrite(RELAY_2, HIGH);
    digitalWrite(RELAY_3, HIGH);
    digitalWrite(RELAY_4, HIGH);

    // Initialize sensors
    dht.begin();

    // Network setup
    setupWiFi();
    setupMQTT();
    setupWebServer();
    setupOTA();

    Serial.println("[OK] System initialized successfully!");
    Serial.println("[INFO] Dashboard: http://" + WiFi.localIP().toString());
}

// ==================== Main Loop ====================
void loop() {
    // Maintain connections
    if (WiFi.status() != WL_CONNECTED) {
        setupWiFi();
    }
    if (!mqtt.connected()) {
        setupMQTT();
    }
    mqtt.loop();
    server.handleClient();
    ArduinoOTA.handle();

    // Read sensors periodically
    if (millis() - lastSensorRead >= SENSOR_INTERVAL) {
        readSensors();
        lastSensorRead = millis();
    }

    // Publish to MQTT periodically
    if (millis() - lastMqttPublish >= MQTT_INTERVAL) {
        publishSensorData();
        lastMqttPublish = millis();
    }

    // Auto-control: Turn on light if dark and motion detected
    if (lightLevel < 500 && motionDetected && !relay1State) {
        setRelay(1, true);
        Serial.println("[AUTO] Light ON — motion detected in dark");
    }

    // Auto-control: Turn on fan if temperature > 30°C
    if (temperature > 30.0 && !relay2State) {
        setRelay(2, true);
        Serial.println("[AUTO] Fan ON — temperature > 30°C");
    } else if (temperature < 26.0 && relay2State) {
        setRelay(2, false);
        Serial.println("[AUTO] Fan OFF — temperature < 26°C");
    }
}

// ==================== WiFi Setup ====================
void setupWiFi() {
    Serial.print("[WiFi] Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n[WiFi] Connected!");
        Serial.println("[WiFi] IP: " + WiFi.localIP().toString());
        digitalWrite(STATUS_LED, HIGH);
    } else {
        Serial.println("\n[WiFi] Connection failed!");
    }
}

// ==================== MQTT Setup ====================
void setupMQTT() {
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);

    int attempts = 0;
    while (!mqtt.connected() && attempts < 5) {
        Serial.print("[MQTT] Connecting...");
        if (mqtt.connect(MQTT_CLIENT)) {
            Serial.println(" Connected!");
            mqtt.subscribe(MQTT_TOPIC_SUB);
            Serial.println("[MQTT] Subscribed to: " + String(MQTT_TOPIC_SUB));
        } else {
            Serial.println(" Failed (rc=" + String(mqtt.state()) + ")");
            delay(2000);
            attempts++;
        }
    }
}

// ==================== MQTT Callback ====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.println("[MQTT] Received: " + message);

    // Parse JSON command
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (error) return;

    if (doc.containsKey("relay1")) setRelay(1, doc["relay1"]);
    if (doc.containsKey("relay2")) setRelay(2, doc["relay2"]);
    if (doc.containsKey("relay3")) setRelay(3, doc["relay3"]);
    if (doc.containsKey("relay4")) setRelay(4, doc["relay4"]);
}

// ==================== Sensor Reading ====================
void readSensors() {
    temperature = dht.readTemperature();
    humidity    = dht.readHumidity();
    lightLevel  = analogRead(LDR_PIN);
    motionDetected = digitalRead(PIR_PIN);

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("[WARN] DHT read failed!");
        temperature = 0;
        humidity = 0;
    }
}

// ==================== MQTT Publish ====================
void publishSensorData() {
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["humidity"]    = humidity;
    doc["light"]       = lightLevel;
    doc["motion"]      = motionDetected;
    doc["relay1"]      = relay1State;
    doc["relay2"]      = relay2State;
    doc["relay3"]      = relay3State;
    doc["relay4"]      = relay4State;

    char buffer[256];
    serializeJson(doc, buffer);
    mqtt.publish(MQTT_TOPIC_PUB, buffer);
}

// ==================== Relay Control ====================
void setRelay(int relay, bool state) {
    int pin;
    switch (relay) {
        case 1: pin = RELAY_1; relay1State = state; break;
        case 2: pin = RELAY_2; relay2State = state; break;
        case 3: pin = RELAY_3; relay3State = state; break;
        case 4: pin = RELAY_4; relay4State = state; break;
        default: return;
    }
    digitalWrite(pin, state ? LOW : HIGH); // Active LOW relays
    Serial.printf("[RELAY] Channel %d: %s\n", relay, state ? "ON" : "OFF");
}

// ==================== Web Server ====================
void setupWebServer() {
    server.on("/", handleRoot);
    server.on("/api/status", handleAPI);
    server.on("/api/control", HTTP_POST, handleControl);
    server.begin();
    Serial.println("[WEB] Server started on port 80");
}

void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Home Dashboard</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', sans-serif;
            background: linear-gradient(135deg, #0f0c29, #302b63, #24243e);
            color: #fff; min-height: 100vh; padding: 20px;
        }
        h1 { text-align: center; margin: 20px 0; font-size: 28px;
             background: linear-gradient(90deg, #00d2ff, #3a7bd5);
             -webkit-background-clip: text; -webkit-text-fill-color: transparent; }
        .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
                gap: 20px; max-width: 1000px; margin: 0 auto; }
        .card { background: rgba(255,255,255,0.1); border-radius: 16px;
                padding: 24px; backdrop-filter: blur(10px);
                border: 1px solid rgba(255,255,255,0.15);
                transition: transform 0.3s; }
        .card:hover { transform: translateY(-4px); }
        .card h3 { font-size: 14px; opacity: 0.7; margin-bottom: 8px; }
        .card .value { font-size: 36px; font-weight: 700; }
        .card .unit { font-size: 16px; opacity: 0.6; }
        .toggle { width: 60px; height: 30px; border-radius: 15px;
                  background: #555; border: none; cursor: pointer;
                  position: relative; transition: background 0.3s; }
        .toggle.active { background: #00d2ff; }
        .toggle::after { content: ''; position: absolute; width: 24px;
                         height: 24px; border-radius: 50%; background: #fff;
                         top: 3px; left: 3px; transition: transform 0.3s; }
        .toggle.active::after { transform: translateX(30px); }
        .relay-row { display: flex; justify-content: space-between;
                     align-items: center; padding: 12px 0;
                     border-bottom: 1px solid rgba(255,255,255,0.1); }
        .footer { text-align: center; margin-top: 30px; opacity: 0.5; font-size: 12px; }
    </style>
</head>
<body>
    <h1>🏠 Smart Home Dashboard</h1>
    <div class="grid">
        <div class="card">
            <h3>🌡️ TEMPERATURE</h3>
            <div class="value" id="temp">--</div>
            <div class="unit">°C</div>
        </div>
        <div class="card">
            <h3>💧 HUMIDITY</h3>
            <div class="value" id="hum">--</div>
            <div class="unit">%</div>
        </div>
        <div class="card">
            <h3>☀️ LIGHT LEVEL</h3>
            <div class="value" id="light">--</div>
            <div class="unit">lux (raw)</div>
        </div>
        <div class="card">
            <h3>🚶 MOTION</h3>
            <div class="value" id="motion">--</div>
        </div>
        <div class="card" style="grid-column: span 2;">
            <h3>⚡ APPLIANCE CONTROL</h3>
            <div class="relay-row">
                <span>💡 Light</span>
                <button class="toggle" id="r1" onclick="toggleRelay(1)"></button>
            </div>
            <div class="relay-row">
                <span>🌀 Fan</span>
                <button class="toggle" id="r2" onclick="toggleRelay(2)"></button>
            </div>
            <div class="relay-row">
                <span>❄️ AC</span>
                <button class="toggle" id="r3" onclick="toggleRelay(3)"></button>
            </div>
            <div class="relay-row">
                <span>🔌 Spare</span>
                <button class="toggle" id="r4" onclick="toggleRelay(4)"></button>
            </div>
        </div>
    </div>
    <div class="footer">Smart Home Automation System — Daggolu Hari Krishna</div>
    <script>
        function updateDashboard() {
            fetch('/api/status').then(r => r.json()).then(d => {
                document.getElementById('temp').textContent = d.temperature.toFixed(1);
                document.getElementById('hum').textContent = d.humidity.toFixed(1);
                document.getElementById('light').textContent = d.light;
                document.getElementById('motion').textContent = d.motion ? 'Detected' : 'None';
                ['r1','r2','r3','r4'].forEach((id,i) => {
                    document.getElementById(id).classList.toggle('active', d['relay'+(i+1)]);
                });
            });
        }
        function toggleRelay(n) {
            const btn = document.getElementById('r'+n);
            const state = !btn.classList.contains('active');
            fetch('/api/control', {
                method: 'POST', headers: {'Content-Type':'application/json'},
                body: JSON.stringify({['relay'+n]: state})
            }).then(() => updateDashboard());
        }
        setInterval(updateDashboard, 2000);
        updateDashboard();
    </script>
</body>
</html>
)rawliteral";
    server.send(200, "text/html", html);
}

void handleAPI() {
    server.send(200, "application/json", getStatusJSON());
}

void handleControl() {
    String body = server.arg("plain");
    StaticJsonDocument<256> doc;
    deserializeJson(doc, body);
    if (doc.containsKey("relay1")) setRelay(1, doc["relay1"]);
    if (doc.containsKey("relay2")) setRelay(2, doc["relay2"]);
    if (doc.containsKey("relay3")) setRelay(3, doc["relay3"]);
    if (doc.containsKey("relay4")) setRelay(4, doc["relay4"]);
    server.send(200, "application/json", getStatusJSON());
}

String getStatusJSON() {
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["humidity"]    = humidity;
    doc["light"]       = lightLevel;
    doc["motion"]      = motionDetected;
    doc["relay1"]      = relay1State;
    doc["relay2"]      = relay2State;
    doc["relay3"]      = relay3State;
    doc["relay4"]      = relay4State;
    String output;
    serializeJson(doc, output);
    return output;
}

// ==================== OTA Setup ====================
void setupOTA() {
    ArduinoOTA.setHostname("SmartHome-ESP32");
    ArduinoOTA.onStart([]() { Serial.println("[OTA] Update starting..."); });
    ArduinoOTA.onEnd([]() { Serial.println("[OTA] Update complete!"); });
    ArduinoOTA.onError([](ota_error_t error) { Serial.printf("[OTA] Error: %u\n", error); });
    ArduinoOTA.begin();
    Serial.println("[OTA] Ready");
}
