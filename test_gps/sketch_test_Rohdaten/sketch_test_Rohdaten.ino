#include <SoftwareSerial.h>

// RX = Pin 4 (GPS TX anschließen), TX = Pin 3 (nicht verwendet)
SoftwareSerial ss(4, 3);

void setup() {
  Serial.begin(9600);   // Serielle Verbindung zum PC
    while (!Serial) {
    ; // Warte
  }
  ss.begin(9600);       // Serielle Verbindung zum GPS-Modul
  Serial.println("Starte GPS-Rohdaten-Test...");
}

void loop() {
  while (ss.available()) {
    char c = ss.read();
    Serial.write(c); // Gibt jedes empfangene Zeichen direkt aus
  }
}
