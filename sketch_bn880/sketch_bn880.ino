#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_HMC5883_U.h>

// GPS über SoftwareSerial (D2 = RX, D3 = TX)
SoftwareSerial gpsSerial(2, 3);
TinyGPSPlus gps;

// Kompass
Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified(12345);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  // Kompass initialisieren
  if (!compass.begin()) {
    Serial.println("HMC5883L nicht gefunden. Bitte Verkabelung prüfen!");
    while (1);
  }

  Serial.println("BN-880 GPS + HMC5883L Kompass gestartet");
}

void loop() {
  // GPS-Daten lesen
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // GPS-Daten anzeigen
  if (gps.location.isUpdated()) {
    Serial.print("GPS: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(", ");
    Serial.print(gps.location.lng(), 6);
    Serial.print(" | Höhe: ");
    Serial.print(gps.altitude.meters());
    Serial.print(" m | Satelliten: ");
    Serial.println(gps.satellites.value());
  }

  // Kompass-Daten lesen
  sensors_event_t event;
  compass.getEvent(&event);

  float heading = atan2(event.magnetic.y, event.magnetic.x);
  if (heading < 0) heading += 2 * PI;
  float headingDegrees = heading * 180 / PI;

  Serial.print("Kompass: ");
  Serial.print(headingDegrees, 2);
  Serial.println("°");

  delay(1000);
}
