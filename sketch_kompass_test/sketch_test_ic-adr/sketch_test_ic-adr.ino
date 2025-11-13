#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial) {
    ; // Warte
  }
  Serial.println("Suche nach I2C-Geräten...");
  
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("I2C-Gerät gefunden bei Adresse 0x");
      Serial.println(address, HEX);
    }
  }
}

void loop() {}
