#include <ESP8266WiFi.h>

// --- WLAN Zugangsdaten ---
const char* ssid     = "DEIN_WIFI_NAME";
const char* password = "DEIN_WIFI_PASSWORT";

// --- LED Pin (D4 = GPIO2, eingebaute LED beim D1 Mini) ---
const int LED_PIN = 2;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(LED_PIN, OUTPUT);

  Serial.println();
  Serial.println("Starte ESP8266 Testprogramm...");

  // --- WLAN verbinden ---
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // LED blinkt als Lebenszeichen
  digitalWrite(LED_PIN, LOW);   // LED an
  delay(500);
  digitalWrite(LED_PIN, HIGH);  // LED aus
  delay(500);

  // Status im seriellen Monitor
  Serial.println("ESP8266 läuft...");
}