const int sensorPin = A0;  // Feuchtigkeitssensor an Analog-Pin A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int moisture = analogRead(sensorPin);
  Serial.print("Feuchtigkeit: ");
  Serial.println(moisture);
  delay(1000);
}
