void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // Warte
  }
  Serial.println("Hallo Jens! Der serielle Monitor funktioniert.");
}

void loop() {
  // kein weiterer Code nötig
}
