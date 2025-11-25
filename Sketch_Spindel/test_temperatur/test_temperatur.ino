#include <OneWire.h>
#include <DallasTemperature.h>

// Pin-Definition: D4 = GPIO2
#define ONE_WIRE_PIN 2

// OneWire und DallasTemperature Objekte
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  Serial.println("Starte DS18B20 Test...");

  sensors.begin();

  // Prüfen, ob Sensor gefunden wurde
  if (sensors.getDeviceCount() == 0) {
    Serial.println("Kein DS18B20 erkannt!");
  } else {
    Serial.print("Gefundene Sensoren: ");
    Serial.println(sensors.getDeviceCount());
  }
}

void loop() {
  sensors.requestTemperatures(); // Temperatur anfordern
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == -127.00) {
    Serial.println("Fehler: Sensor nicht ausgelesen (-127 °C)");
  } else {
    Serial.print("Temperatur: ");
    Serial.print(tempC);
    Serial.println(" °C");
  }

  delay(2000); // alle 2 Sekunden aktualisieren
}