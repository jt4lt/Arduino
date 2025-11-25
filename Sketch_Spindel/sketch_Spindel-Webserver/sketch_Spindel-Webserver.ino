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
#define MAX_SAMPLES 1000
float tempHistory[MAX_SAMPLES];
float angleHistory[MAX_SAMPLES];
String timeHistory[MAX_SAMPLES];
int sampleIndex = 0;

// Konfigurationswerte
float angleOffset = 0.0;
float tempOffset = 0.0;
unsigned long measureInterval = 10000; // 10 Sekunden
unsigned long lastMeasure = 0;

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

  unsigned long seconds = millis() / 1000;
  timeHistory[sampleIndex] = String(seconds) + "s";

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

  // Hauptseite
  server.on("/", []() {
    String html = "<html><head><meta charset='UTF-8'><title>Cidre-Spindel</title>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>"; // Handy-Optimierung
    html += "<style>";
    html += "body{font-family:sans-serif;margin:10px;}h1{text-align:center;font-size:20px;}";
    html += "form input{width:100%;margin:5px 0;padding:6px;font-size:14px;}";
    html += "canvas{width:100%!important;height:100%!important;}";
    html += "table{width:100%;border-collapse:collapse;font-size:14px;margin-top:15px;}";
    html += "th,td{border:1px solid #ccc;padding:4px;text-align:center;}";
    html += "</style>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
    html += "<h1>Cidre Gärspindel</h1>";

    // Konfigurationsformular
    html += "<form action='/config' method='GET'>";
    html += "Winkel-Offset: <input type='number' step='0.1' name='angle' value='" + String(angleOffset) + "'><br>";
    html += "Temperatur-Offset: <input type='number' step='0.1' name='temp' value='" + String(tempOffset) + "'><br>";
    html += "Intervall (ms): <input type='number' name='interval' value='" + String(measureInterval) + "'><br>";
    html += "<input type='submit' value='Speichern'></form>";

    // Chart.js oben, responsive Container
    html += "<div style='width:100%;height:300px;'><canvas id='chart'></canvas></div>";
    html += "<script>";
    html += "const ctx = document.getElementById('chart').getContext('2d');";
    html += "new Chart(ctx, {type: 'line', data: {labels: [";

    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += "'" + timeHistory[i] + "'";
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], datasets: [{label: 'Temperatur (°C)', data: [";
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += String(tempHistory[i]);
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], borderColor: 'red', fill: false}, {label: 'Winkel (°)', data: [";
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += String(angleHistory[i]);
      if (i < MAX_SAMPLES - 1) html += ",";
    }

    html += "], borderColor: 'blue', fill: false}]}, options:{responsive:true,maintainAspectRatio:false,scales:{x:{title:{display:true,text:'Zeit'}},y:{title:{display:true,text:'Wert'}}}}});";
    html += "</script>";

    // Tabelle mit Messwerten
    html += "<h2>Messwerte</h2><table><tr><th>Zeit</th><th>Temperatur (°C)</th><th>Winkel (°)</th></tr>";
    for (int i = 0; i < MAX_SAMPLES; i++) {
      html += "<tr><td>" + timeHistory[i] + "</td><td>" + String(tempHistory[i]) + "</td><td>" + String(angleHistory[i]) + "</td></tr>";
    }
    html += "</table>";

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  // Konfigurationsroute
  server.on("/config", []() {
    if (server.hasArg("angle")) angleOffset = server.arg("angle").toFloat();
    if (server.hasArg("temp")) tempOffset = server.arg("temp").toFloat();
    if (server.hasArg("interval")) measureInterval = server.arg("interval").toInt();

    server.sendHeader("Location", "/");
    server.send(303); // Redirect zurück zur Hauptseite
  });

  // Server starten
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - lastMeasure >= measureInterval) {
    lastMeasure = millis();

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0) + tempOffset;

    float angle = calculateAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z) + angleOffset;

    addSample(tempC, angle);
  }
}