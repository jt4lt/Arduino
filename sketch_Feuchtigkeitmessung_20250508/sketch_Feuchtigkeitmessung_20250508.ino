const int sensorPin = 1;       // GPIO1 für Feuchtigkeitssensor (anpassen!)
const int pumpPin = 10;        // GPIO10 für Pumpensteuerung (anpassen!)
const int threshold = 2000;    // Schwellwert für Trockenheit (anpassen!)
const int pumpDuration = 5000; // Pumpzeit in Millisekunden (z. B. 5 Sekunden)

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Pumpe aus
}

void loop() {
  int moisture = analogRead(sensorPin);
  Serial.print("Feuchtigkeit: ");
  Serial.println(moisture);

  if (moisture < threshold) {
    Serial.println("Zu trocken! Pumpe wird aktiviert.");
    digitalWrite(pumpPin, HIGH);
    delay(pumpDuration);
    digitalWrite(pumpPin, LOW);
    Serial.println("Pumpe gestoppt.");
  }

  delay(10000); // 10 Sekunden warten bis zur nächsten Messung
}
