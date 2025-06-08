void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Serieller Test gestartet");
}

void loop() {
  Serial.println("Hallo von deinem ESP32!");
  delay(1000); // Warte 1 Sekunde
}
