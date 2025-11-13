#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <QMC5883LCompass.h>

// GPS über SoftwareSerial (D2 = RX, D3 = TX)
SoftwareSerial gpsSerial(2, 3);
TinyGPSPlus gps;

// Kompass
QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);

  Wire.begin();
  compass.init();
  compass.setCalibration(-278, 655, -118, 581, -250, 518); // Optional: Kalibrierung anpassen

  Serial.println("BN-880 GPS + Kompass gestartet");
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
  compass.read();
  int heading = compass.getAzimuth();
  String direction = compass.getDirection();

  Serial.print("Kompass: ");
  Serial.print(heading);
  Serial.print("° ");
  Serial.println(direction);

  delay(1000);
}
