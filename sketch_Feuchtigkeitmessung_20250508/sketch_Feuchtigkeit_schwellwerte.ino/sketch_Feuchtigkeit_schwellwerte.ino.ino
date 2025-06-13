const int sensorPin = 1;         // GPIO1 für Feuchtigkeitssensor
const int pumpPin = 10;          // GPIO10 für Pumpensteuerung
const int potiThresholdPin = 2;  // GPIO2 für Schwellwert-Poti
const int potiDurationPin = 3;   // GPIO3 für Dauer-Poti

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Pumpe aus
}

void loop() {
  int moisture = analogRead(sensorPin);
  int potiThreshold = analogRead(potiThresholdPin);
  int potiDuration = analogRead(potiDurationPin);

  // Schwellwert zwischen 0 und 4095 (je nach ADC-Auflösung)
  int threshold = map(potiThreshold, 0, 4095, 1000, 3000); // z. B. von 1000 bis 3000
  // Pumpdauer zwischen 1 und 10 Sekunden
  int pumpDuration = map(potiDuration, 0, 4095, 1000, 10000); // in Millisekunden

  Serial.print("Feuchtigkeit: ");
  Serial.print(moisture);
  Serial.print(" | Schwellwert: ");
  Serial.print(threshold);
  Serial.print(" | Pumpdauer: ");
  Serial.println(pumpDuration);

  if (moisture < threshold) {
    Serial.println("Zu trocken! Pumpe wird aktiviert.");
    digitalWrite(pumpPin, HIGH);
    delay(pumpDuration);
    digitalWrite(pumpPin, LOW);
    Serial.println("Pumpe gestoppt.");
  }

  delay(10000); // 10 Sekunden warten bis zur nächsten Messung
}
