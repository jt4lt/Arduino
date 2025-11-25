#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WLAN Zugangsdaten
const char* ssid = "wirhier";
const char* password = "NiToLaNi4!2908PMOOR";

// Webserver auf Port 80
ESP8266WebServer server(80);

// MPU6050
Adafruit_MPU6050 mpu;

// DS18B20
#define ONE_WIRE_PIN 2 // D4 (GPIO2)
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// Speicher für Verlauf
#define MAX_SAMPLES 50
float tempHistory[MAX_SAMPLES];
float angleHistory[MAX_SAMPLES];
int sampleIndex = 0;

// Hilfsfunktion: Winkel aus Beschleunigungswerten berechnen
float calculateAngle(float x, float y, float z) {
  float angle = atan2(x, z) * 180.0 / PI;
  if (angle < 0) angle += 180; // nur positive Werte
  return angle;
}

// Neue Werte ins Array einfügen (Ringpuffer)
void addSample(float temp, float angle) {
  tempHistory[sampleIndex] = temp;
  angleHistory[sampleIndex] = angle;
  sampleIndex = (sampleIndex + 1) % MAX_SAMPLES;
}

void setup() {
  Serial.begin(115200);

  // WLAN verbinden
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // DS18B20 starten
  sensors.begin();

  // I²C starten für MPU6050 (SDA=D2=GPIO4, SCL=D1=GPIO5)
  Wire.begin(4, 5);

  // MPU6050 starten
  if (!mpu.begin()) {
    Serial.println("Fehler: MPU6050 nicht gefunden!");
  } else {
    Serial.println("MPU6050 erfolgreich gefunden!");
  }

  // Webserver Route
  server.on("/", []() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    // Winkel berechnen
    float angle = calculateAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z);

    // Gärungsgrad (0–100 %)
    float fermentationProgress = (angle / 90.0) * 100.0;
    if (fermentationProgress > 100) fermentationProgress = 100;

    // Werte speichern
    addSample(tempC, angle);

    // HTML-Seite mit Chart.js
    String html = "<html><head><meta charset='UTF-8'><title>Cidre-Spindel</title>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
    html += "<h1>Cidre Gärspindel</h1>";
    html += "<p>Temperatur: " + String(tempC) + " °C</p>";
    html += "<p>Winkel: " + String(angle) + " °</p>";
    html += "<p>Gärungsgrad: " + String(fermentationProgress, 1) + " %</p>";
    html += "<canvas id='chart' width='400' height='200'></canvas>";
    html += "<script>";
    html += "const ctx = document.getElementById('chart').getContext('2d');";
    html += "new Chart(ctx, {type: 'line', data: {labels: [";

    // Labels (0..MAX_SAMPLES-1)
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += "'" + String(i) + "'";
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], datasets: [{label: 'Temperatur (°C)', data: [";

    // Temperaturwerte
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += String(tempHistory[i]);
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], borderColor: 'red', fill: false}, {label: 'Winkel (°)', data: [";

    // Winkelwerte
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += String(angleHistory[i]);
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], borderColor: 'blue', fill: false}]}});";
    html += "</script></body></html>";

    server.send(200, "text/html", html);
  });

  // Server starten
  server.begin();
}

void loop() {
  server.handleClient();
}