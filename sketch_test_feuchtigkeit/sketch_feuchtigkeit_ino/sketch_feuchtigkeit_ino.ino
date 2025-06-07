// Pin-Konfiguration
const int sensorPin = 34;  // ADC-Pin für Feuchtigkeitssensor
const int pumpPin = 26;    // GPIO zur Steuerung der Pumpe (z. B. über MOSFET oder Relais)

// Schwellenwert und Pumpdauer
const int dryThreshold = 2000;            // ADC-Wert unterhalb dessen die Erde als "trocken" gilt
const unsigned long pumpDuration = 5000;  // Pumpdauer in Millisekunden (z. B. 5000 = 5 Sekunden)

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Pumpe aus
}

void loop() {
  int moisture = analogRead(sensorPin);
  Serial.print("Feuchtigkeitswert: ");
  Serial.println(moisture);

  if (moisture < dryThreshold) {
    Serial.println("Erde ist zu trocken. Pumpe wird aktiviert.");
    digitalWrite(pumpPin, HIGH);  // Pumpe an
    delay(pumpDuration);
    digitalWrite(pumpPin, LOW);  // Pumpe aus
    Serial.println("Pumpe ausgeschaltet.");
  }

  delay(10000);  // 10 Sekunden warten bis zur nächsten Messung
}
