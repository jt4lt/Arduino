#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// GPS an Pin 4 (RX), Pin 3 (TX ungenutzt)
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println("Starte GPS-Datenanzeige...");
}

void loop() {
  // GPS-Daten einlesen
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }

  // Wenn neue Standortdaten verfügbar sind
  if (gps.location.isUpdated()) {
    Serial.println("----- GPS-Daten -----");
    Serial.print("Breite: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Länge: ");
    Serial.println(gps.location.lng(), 6);

    if (gps.altitude.isValid()) {
      Serial.print("Höhe: ");
      Serial.print(gps.altitude.meters());
      Serial.println(" m");
    }

    if (gps.date.isValid()) {
      Serial.print("Datum: ");
      Serial.print(gps.date.day());
      Serial.print(".");
      Serial.print(gps.date.month());
      Serial.print(".");
      Serial.println(gps.date.year());
    }

    if (gps.time.isValid()) {
      Serial.print("Uhrzeit (UTC): ");
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.println(gps.time.second());
    }

    Serial.println("---------------------\n");
    delay(1000); // Warte 1 Sekunde bis zur nächsten Anzeige
  }
}

