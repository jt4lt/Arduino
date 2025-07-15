#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = 1;
const int pumpPin = 10;
const int potiThresholdPin = 2;
const int potiDurationPin = 3;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

unsigned long lastUpdate = 0;
unsigned long updateInterval = 1000; // 1 Sekunde für schnellere Reaktion bei Potiänderung

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  Wire.begin(21, 22); // SDA, SCL
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System startet...");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // Display wird jede Sekunde aktualisiert
  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;

    int moisture = analogRead(sensorPin);
    int potiThreshold = analogRead(potiThresholdPin);
    int potiDuration = analogRead(potiDurationPin);

    int threshold = map(potiThreshold, 0, 4095, 1000, 3000);
    int pumpDuration = map(potiDuration, 0, 4095, 10000, 100000);

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
    lcd.print("Dauer:");
    lcd.print(pumpDuration / 1000); // in Sekunden
    lcd.print("s");

    if (moisture < threshold) {
      Serial.println("Zu trocken! Pumpe wird aktiviert.");
      digitalWrite(pumpPin, HIGH);
      delay(pumpDuration);
      digitalWrite(pumpPin, LOW);
      Serial.println("Pumpe gestoppt.");
    }
  }
}
