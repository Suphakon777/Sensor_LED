#include <WiFi.h>
#include <HTTPClient.h>

#define TRIG_PIN 23
#define ECHO_PIN 22
#define LED_PIN 21

// WiFi credentials
const char* ssid = "Mokejoo";
const char* password = "77881133";

// Webhook URL
const char* webhookURL = "https://webhook.site/ceea0145-76fb-43ef-b614-f51c08ece301";

// Person detection threshold (adjust based on your setup)
const int detectionThreshold = 50; // in cm

// Tracking variables
int peopleCount = 0;
bool personDetected = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");
}

void loop() {
  float distance = measureDistance();

  if (distance > 0 && distance < detectionThreshold) {
    if (!personDetected) {
      personDetected = true;
      peopleCount++;
      digitalWrite(LED_PIN, HIGH);
      Serial.print("Person detected! Total: ");
      Serial.println(peopleCount);
      sendWebhook(peopleCount);
    }
  } else {
    personDetected = false;
    digitalWrite(LED_PIN, LOW);
  }

  delay(200);
}

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout at 30ms (5m)
  if (duration == 0) return -1; // No reading
  float distance = duration * 0.0343 / 2;
  return distance;
}

void sendWebhook(int count) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(webhookURL);
    http.addHeader("Content-Type", "application/json");

    String payload = "{\"people_count\": " + String(count) + "}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      Serial.println("Webhook sent! Code: " + String(httpResponseCode));
    } else {
      Serial.println("Failed to send webhook.");
    }

    http.end();
  }
}
