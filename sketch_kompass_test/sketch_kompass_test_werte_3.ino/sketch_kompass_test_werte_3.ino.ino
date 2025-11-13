#include <Wire.h>

#define COMPASS_ADDR 0x1E  // Standardadresse des HMC5883L (GY-271)
//#define COMPASS_ADDR 0x2  //Wenn der Kompass eine andere Adresse hat

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Optional: Initialisierung des Kompasses
  // Einige Kompasse benötigen eine Initialisierung, um korrekt zu funktionieren.
  // Wenn der Kompass nicht auf Standardwerte reagiert, können diese Initialisierungswerte
  // verwendet werden. Diese sollten im Datenblatt des Kompasses gefunden werden.
  //  writeRegister(0x02, 0b00011000); // Set Continuous-Measurement Mode
}

void loop() {
  // Daten vom Kompass lesen
  int x, y, z;
  readCompass(&x, &y, &z);

  // Berechne den Winkel
  float heading = atan2(y, x) * 180.0 / PI;

  // Passe den Winkel an, um den Kompass korrekt auszurichten
  if (heading < 0) {
    heading += 360.0;
  }

  // Ausgabe der Daten
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.print(z);
  Serial.print("  Heading: ");
  Serial.print(heading);
  Serial.println(" Grad");

  delay(100); // Kleine Pause, um die Ausgabe zu verlangsamen und Daten besser lesbar zu machen
}

// Funktion zum Lesen der Daten vom Kompass
void readCompass(int *x, int *y, int *z) {
  // Drei Datenregister lesen (X, Y, Z)
  Wire.beginTransmission(COMPASS_ADDR);
  Wire.write(0x03); // Start register (X MSB)
  Wire.endTransmission();

  Wire.requestFrom(COMPASS_ADDR, 6); // Fordere 6 Bytes an (X MSB, X LSB, Y MSB, Y LSB, Z MSB, Z LSB)
  if (Wire.available() >= 6) {
    *x = (Wire.read() << 8) | Wire.read(); // X-Achse
    *y = (Wire.read() << 8) | Wire.read(); // Y-Achse
    *z = (Wire.read() << 8) | Wire.read(); // Z-Achse
  }
}

// Funktion zum Schreiben in ein Register des Kompasses
void writeRegister(byte reg, byte val) {
  Wire.beginTransmission(COMPASS_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}
