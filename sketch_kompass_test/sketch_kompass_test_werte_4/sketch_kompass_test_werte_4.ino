#include <Wire.h>

#define QMC5883L_ADDR 0x2C  // Angepasste Adresse

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialisierung des QMC5883L
  Wire.beginTransmission(QMC5883L_ADDR);
  Wire.write(0x0B); // Control Register
  Wire.write(0b00011101); // Continuous mode, 200Hz, 2G range, 128 oversampling
  Wire.endTransmission();
}

void loop() {
  int16_t x, y, z;

  Wire.beginTransmission(QMC5883L_ADDR);
  Wire.write(0x00); // Datenregister starten bei 0x00
  Wire.endTransmission();

  Wire.requestFrom(QMC5883L_ADDR, 6);
  if (Wire.available() == 6) {
    x = Wire.read() | (Wire.read() << 8);
    y = Wire.read() | (Wire.read() << 8);
    z = Wire.read() | (Wire.read() << 8);

    float heading = atan2((float)y, (float)x) * 180.0 / PI;
    if (heading < 0) heading += 360;

    Serial.print("X: "); Serial.print(x);
    Serial.print(" Y: "); Serial.print(y);
    Serial.print(" Z: "); Serial.print(z);
    Serial.print(" Heading: "); Serial.print(heading);
    Serial.println("°");
  }

  delay(200);
}
