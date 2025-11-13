#include <Wire.h>
#include <DFRobot_QMC5883.h>

DFRobot_QMC5883 compass;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialisiere den Kompass
  if (!compass.begin()) {
    Serial.println("Kompass nicht gefunden. Bitte Verbindung prüfen.");
    while (1);
  }

  // Konfiguration des Kompasses
  compass.setRange(QMC5883_RANGE_2GA);
  compass.setMeasurementMode(QMC5883_CONTINOUS);
  compass.setDataRate(QMC5883_DATARATE_50HZ);
  compass.setSamples(QMC5883_SAMPLES_8);
}

void loop() {
  sVector_t mag = compass.readRaw();

  Serial.print("X: ");
  Serial.print(mag.XAxis);
  Serial.print(" | Y: ");
  Serial.print(mag.YAxis);
  Serial.print(" | Z: ");
  Serial.println(mag.ZAxis);

  delay(500);
}
