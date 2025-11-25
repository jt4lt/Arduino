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
#define ONE_WIRE_PIN 2 // D4 (GPIO4) empfohlen
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

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

    String html = "<html><head><meta charset='UTF-8'><title>Bier-Spindel</title></head><body>";
    html += "<h1>Tobias seine Spindel</h1>";
    html += "<p>Temperatur: " + String(tempC) + " °C</p>";
    html += "<p>Accel X: " + String(a.acceleration.x) + " m/s²</p>";
    html += "<p>Accel Y: " + String(a.acceleration.y) + " m/s²</p>";
    html += "<p>Accel Z: " + String(a.acceleration.z) + " m/s²</p>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.begin();
}

void loop() {
  server.handleClient();
}