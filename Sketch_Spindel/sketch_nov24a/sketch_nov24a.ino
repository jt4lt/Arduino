#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// --- MPU6050 Setup ---
Adafruit_MPU6050 mpu;

// --- DS18B20 Setup ---
#define ONE_WIRE_BUS 2   // DS18B20 Signal-Pin an D2 (NodeMCU: GPIO4)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // MPU6050 starten
  if (!mpu.begin()) {
    Serial.println("Fehler: MPU6050 nicht gefunden!");
    while (1) { delay(10); }
  }
  Serial.println("MPU6050 verbunden.");

  // MPU6050 Konfiguration
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // DS18B20 starten
  sensors.begin();
  Serial.println("DS18B20 gestartet.");
}

void loop() {
  // --- MPU6050 Werte ---
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Serial.print("MPU6050 -> Accel: ");
  Serial.print("X="); Serial.print(a.acceleration.x);
  Serial.print(" Y="); Serial.print(a.acceleration.y);
  Serial.print(" Z="); Serial.print(a.acceleration.z);

  Serial.print(" | Gyro: ");
  Serial.print("X="); Serial.print(g.gyro.x);
  Serial.print(" Y="); Serial.print(g.gyro.y);
  Serial.print(" Z="); Serial.println(g.gyro.z);

  // --- DS18B20 Temperatur ---
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Serial.print("DS18B20 -> Temperatur: ");
  Serial.print(tempC);
  Serial.println(" °C");

  delay(1000); // 1 Sekunde Pause
}