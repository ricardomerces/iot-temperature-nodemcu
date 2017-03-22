/*
 * ESP8266 (nodemcu) + MQTT + DHT for IoT
 * V 1.1 - Ricardo Merces
 */

// Depends on the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - MQTT Client Library: https://github.com/knolleary/pubsubclient


// Set configuration  for WiFi, MQTT, and DHT
// WiFi        = WIFI_SSID, WIFI_PASS
// MQTT        = MQTT_HOST; MQTT_USER; MQTT_PASS; MQTT_CLIENT_ID 
// MQTT Topics = MQTT_TOPIC_TEMP; MQTT_TOPIC_HUMID
// DHT         = DHT_PIN; DHT_SAMPLE_DELAY

// --- WiFi ---
#include <ESP8266WiFi.h>
const char* wifiSSID = "WIFI_SSID";                   // Your SSID       
const char* wifiPassword = "WIFI_PASS";               // Your Password
WiFiClient espClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqttServer = "MQTT_HOST";                // iot.eclipse.org
const char* mqttUsername = "MQTT_USER";              // MQTT User
const char* mqttPassword = "MQTT_PASS";              // MQTT Password
const char* mqttClientId = "MQTT_RMO_TEMP01";        // Must be unique on the MQTT network
PubSubClient client(espClient);

// --- MQTT Topics ---
const char* temperatureTopic = "MQTT_TOPIC_TEMP";    // TOPIC Temperuture - Ex: home/room/temperature
const char* humidityTopic = "MQTT_TOPIC_HUMID";      // TOPIC HUMIDITY - Ex: home/room/humidity

// --- DHT ---
#include <DHT.h>
#define DHTTYPE DHT22                                // Update this to match your DHT type
const int dhtPin = 2;                                // DHT Pin DATA (D2, ...)
const long sampleDelay = 5000;                       // DELAY in Milliseconds
DHT dht(dhtPin, DHTTYPE);
unsigned long lastSampleTime = 0;

// --- SETUP ---
void setup() {
  Serial.begin(115200);                              // COMMENT TO DISABLE SERIAL
  setupWifi();
  client.setServer(mqttServer, 1883);
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  measurementTempHumid();
}

// --- AUXILIARY FUNCTIONS ---

// --- SETUP WiFi ---
void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifiSSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// --- MQTT-RECONNECT CLIENT --- 
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqttClientId)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// --- MEASUREMENT TEMPERATURE & HUMIDITY
void measurementTempHumid() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastSampleTime >= sampleDelay) {
    lastSampleTime = currentMillis;
    float humidity = dht.readHumidity();
    float celsius = dht.readTemperature(false);
    client.publish(temperatureTopic, String(celsius).c_str(), true);    // Keep the message on the Broker (true or false)
    Serial.print("Temperature: ");
    Serial.print(celsius);
    Serial.println(" *C");
    client.publish(humidityTopic, String(humidity).c_str(), true);      // Keep the message on the Broker (true or false)
    Serial.print("Humidity: ");
    Serial.println(humidity);
  }
}

