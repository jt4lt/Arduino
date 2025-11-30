#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// WLAN Zugangsdaten
const char* ssid = "wirhier";
const char* password = "NiToLaNi4!2908PMOOR";

ESP8266WebServer server(80);

// MPU6050
Adafruit_MPU6050 mpu;

// DS18B20
#define ONE_WIRE_PIN 14 // D5 (GPIO14)
OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

// Speicher
#define MAX_SAMPLES 50   // kleiner für Stabilität und Diagrammgröße
float tempHistory[MAX_SAMPLES];
float angleHistory[MAX_SAMPLES];
unsigned long timeHistory[MAX_SAMPLES];
int sampleIndex = 0;

// Konfig
float angleOffset = 0.0;
float tempOffset = 0.0;
unsigned long measureInterval = 10000;
unsigned long lastMeasure = 0;

float calculateAngle(float x, float y, float z) {
  float angle = atan2(x, z) * 180.0 / PI;
  if (angle < 0) angle += 180;
  return angle;
}

void addSample(float temp, float angle) {
  tempHistory[sampleIndex] = temp;
  angleHistory[sampleIndex] = angle;
  timeHistory[sampleIndex] = millis() / 1000;
  sampleIndex = (sampleIndex + 1) % MAX_SAMPLES;
}

void resetSamples() {
  for (int i=0; i<MAX_SAMPLES; i++) {
    tempHistory[i] = 0;
    angleHistory[i] = 0;
    timeHistory[i] = 0;
  }
  sampleIndex = 0;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());

  sensors.begin();
  Wire.begin(4, 5);
  mpu.begin();

  // Hauptseite
  server.on("/", []() {
    String html = "<html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
    html += "<h1>Cidre Gärspindel</h1>";

    // Buttons
    html += "<button onclick=\"window.location='/csv'\">CSV herunterladen</button> ";
    html += "<button onclick=\"window.location='/calibrate'\">Winkel eichen (0°)</button> ";
    html += "<button onclick=\"window.location='/reset'\">Reset Messwerte</button><br><br>";

    // Kompakter Container fürs Diagramm
    html += "<div style='width:100%; max-width:600px; height:300px; margin:auto;'>";
    html += "<canvas id='chart'></canvas></div>";

    html += "<table id='dataTable' border='1' style='width:100%;margin-top:10px;'><tr><th>Zeit</th><th>Temp (°C)</th><th>Winkel (°)</th></tr></table>";

    // JS für Daten laden
    html += "<script>";
    html += "async function loadData(){let r=await fetch('/data');let d=await r.json();";
    html += "let labels=d.time.map(t=>t+'s');";
    html += "if(!window.myChart){const ctx=document.getElementById('chart').getContext('2d');window.myChart=new Chart(ctx,{type:'line',data:{labels:labels,datasets:[{label:'Temp',data:d.temp,borderColor:'red',fill:false},{label:'Winkel',data:d.angle,borderColor:'blue',fill:false}]},options:{responsive:true,maintainAspectRatio:false}});}else{window.myChart.data.labels=labels;window.myChart.data.datasets[0].data=d.temp;window.myChart.data.datasets[1].data=d.angle;window.myChart.update();}";
    html += "let tbl=document.getElementById('dataTable');tbl.innerHTML='<tr><th>Zeit</th><th>Temp (°C)</th><th>Winkel (°)</th></tr>';for(let i=0;i<d.time.length;i++){tbl.innerHTML+=`<tr><td>${d.time[i]}s</td><td>${d.temp[i]}</td><td>${d.angle[i]}</td></tr>`;}}";
    html += "setInterval(loadData,5000);loadData();</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // JSON-Daten
  server.on("/data", []() {
    String json = "{ \"time\":[";
    for (int i=0;i<MAX_SAMPLES;i++){ json += String(timeHistory[i]); if(i<MAX_SAMPLES-1) json+=","; }
    json += "], \"temp\":[";
    for (int i=0;i<MAX_SAMPLES;i++){ json += String(tempHistory[i]); if(i<MAX_SAMPLES-1) json+=","; }
    json += "], \"angle\":[";
    for (int i=0;i<MAX_SAMPLES;i++){ json += String(angleHistory[i]); if(i<MAX_SAMPLES-1) json+=","; }
    json += "] }";
    server.send(200,"application/json",json);
  });

  // CSV-Export
  server.on("/csv", []() {
    String csv = "Zeit (s);Temperatur (°C);Winkel (°)\n";
    for (int i=0;i<MAX_SAMPLES;i++){
      csv += String(timeHistory[i]) + ";" + String(tempHistory[i]) + ";" + String(angleHistory[i]) + "\n";
    }
    server.send(200,"text/csv",csv);
  });

  // Eichung
  server.on("/calibrate", []() {
    sensors_event_t a,g,temp;
    mpu.getEvent(&a,&g,&temp);
    float currentAngle = calculateAngle(a.acceleration.x,a.acceleration.y,a.acceleration.z);
    angleOffset = -currentAngle;  // aktueller Winkel wird als 0 gesetzt
    server.send(200,"text/html","<h1>Winkel auf 0° geeicht!</h1><a href='/'>Zurück</a>");
  });

  // Reset
  server.on("/reset", []() {
    resetSamples();
    server.send(200,"text/html","<h1>Messwerte zurückgesetzt!</h1><a href='/'>Zurück</a>");
  });

  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastMeasure >= measureInterval) {
    lastMeasure = millis();
    sensors_event_t a,g,temp;
    mpu.getEvent(&a,&g,&temp);
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0) + tempOffset;
    float angle = calculateAngle(a.acceleration.x,a.acceleration.y,a.acceleration.z) + angleOffset;
    addSample(tempC, angle);
  }
}