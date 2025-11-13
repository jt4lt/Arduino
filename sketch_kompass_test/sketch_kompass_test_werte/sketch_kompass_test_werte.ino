#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  compass.init();
}

void loop() {
  compass.read();  // Werte intern aktualisieren

  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();

  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.println(z);

  delay(500);
}
