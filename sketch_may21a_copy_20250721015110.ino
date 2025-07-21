#define BLYNK_TEMPLATE_ID "TMPL2b9JnfPM1"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring Syetem"
#define BLYNK_AUTH_TOKEN "cvXfGGTn6HborlSMdVsJ6HexZ7v_HDHI"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char ssid[] = "Tahqiiq System";
char pass[] = "614444259";

// DHT11 config
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// MQ135 pin
#define MQ135_PIN A0

// Buzzer pin
#define BUZZER_PIN D5  // GPIO14

BlynkTimer timer;
bool isGasHigh = false;

void sendSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int gasLevel = analogRead(MQ135_PIN);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ Failed to read DHT11!");
    return;
  }

  // Debug output
  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" % | Gas Level: ");
  Serial.println(gasLevel);

  // Send to Blynk app
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, gasLevel);

  // Gas detection and buzzer + notification
  if (gasLevel > 150 && !isGasHigh) {
    isGasHigh = true;
    Serial.println("🚨 GAS LEVEL HIGH! Sending notification + buzzer ON");
    Blynk.logEvent("gas_alert", "🚨 MQ135: Gas Leakage Detected!");
    digitalWrite(BUZZER_PIN, HIGH);  // Buzzer ON
  }

  if (gasLevel <= 150 && isGasHigh) {
    isGasHigh = false;
    Serial.println("✅ Gas level safe. Turning buzzer OFF");
    digitalWrite(BUZZER_PIN, LOW);   // Buzzer OFF
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  // Buzzer OFF initially

  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}