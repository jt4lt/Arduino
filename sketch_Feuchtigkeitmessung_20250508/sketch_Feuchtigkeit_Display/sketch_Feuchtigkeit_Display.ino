#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = 1;         // GPIO1 für Feuchtigkeitssensor
const int pumpPin = 10;          // GPIO10 für Pumpensteuerung
const int potiThresholdPin = 2;  // GPIO2 für Schwellwert-Poti
const int potiDurationPin = 3;   // GPIO3 für Dauer-Poti

// LCD: Adresse 0x27 oder 0x3F sind üblich. Hier: 0x27 für 16x2 LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW); // Pumpe aus

  Wire.begin(21, 22); // SDA = GPIO21, SCL = GPIO22
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System startet...");
  delay(1000);
  lcd.clear();
}

void loop() {
  int moisture = analogRead(sensorPin);
  int potiThreshold = analogRead(potiThresholdPin);
  int potiDuration = analogRead(potiDurationPin);

  int threshold = map(potiThreshold, 0, 4095, 1000, 3000);
  int pumpDuration = map(potiDuration, 0, 4095, 1000, 10000);

  Serial.print("Feuchtigkeit: ");
  Serial.print(moisture);
  Serial.print(" | Schwellwert: ");
  Serial.print(threshold);
  Serial.print(" | Pumpdauer: ");
  Serial.println(pumpDuration);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("F:");
  lcd.print(moisture);
  lcd.print(" S:");
  lcd.print(threshold);

  lcd.setCursor(0, 1);
  if (moisture < threshold) {
    lcd.print("Pumpe AN ");
    Serial.println("Zu trocken! Pumpe wird aktiviert.");
    digitalWrite(pumpPin, HIGH);
    delay(pumpDuration);
    digitalWrite(pumpPin, LOW);
    Serial.println("Pumpe gestoppt.");
  } else {
    lcd.print("Status: OK");
  }

  delay(10000);
}
