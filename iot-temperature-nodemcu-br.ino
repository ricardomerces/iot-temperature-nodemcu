/*
 * ESP8266 (nodemcu) + MQTT + DHT for IoT
 * V 1.1 - Ricardo Merces
 */

// Dependências - Bibliotecas Arduino:
// - Sensor DHT: https://github.com/adafruit/DHT-sensor-library
// - Cliente MQTT: https://github.com/knolleary/pubsubclient


// Configurações dos parâmentros WiFI, MQTT e DHT
// WiFi        = WIFI_SSID, WIFI_SENHA
// MQTT        = MQTT_BROKER; MQTT_USUARIO; MQTT_SENHA; MQTT_ID_CLIENTE 
// MQTT Topics = MQTT_TOPICO_TEMPERATURA; MQTT_TOPICO_UMIDADE
// DHT         = DHT_PINO; DHT_INTERVALO_MEDICAO

// --- WiFi ---
#include <ESP8266WiFi.h>
const char* wifiSSID = "WIFI_SSID";                  // SSID da sua rede       
const char* wifiPassword = "WIFI_SENHA";             // Senha
WiFiClient espClient;

// --- MQTT ---
#include <PubSubClient.h>
const char* mqttBroker = "MQTT_BROKER";              // iot.eclipse.org
const char* mqttUsuario = "MQTT_USUARIO";            // Usuário MQTT
const char* mqttSenha = "MQTT_SENHA";                // Senha MQTT
const char* mqttClienteId = "MQTT_ID_CLIENTE";       // Nome do cliente
PubSubClient client(espClient);

// --- MQTT Topics ---
const char* topicoTemperatura = "MQTT_TOPICO_TEMPERATURA";   // TOPICO Temperatura - Ex: home/room/temperature
const char* topicoUmidade = "MQTT_TOPICO_UMIDADE";           // TOPICO Umidade - Ex: home/room/humidity

// --- DHT ---
#include <DHT.h>
#define DHTTYPE DHT22                                // Selecione o modelo do seu sensor (DHT11, DHT22, etc)
const int pinDHT = DHT_PINO;                         // Pino de dados do DHT (D2, ...)
const long intervaloMedicao = DHT_INTERVALO_MEDICAO; // Intervalo das medições in Milliseconds EX. 5000 (5 Segundos)
DHT dht(pinDHT, DHTTYPE);
unsigned long ultimaMedicao = 0;

// --- SETUP ---
void setup() {
  Serial.begin(115200);                              // Comentar para habilitar a serial
  conectarWifi();
  client.setServer(mqttBroker, 1883);
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconectarMQTT();
  }
  client.loop();
  medirTemperaturaUmidade();
}

// --- AUXILIARY FUNCTIONS ---

// --- Configuração e Conexão do  WiFi ---
void conectarWifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ");
  Serial.println(wifiSSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

// --- MQTT-RECONNECT CLIENT --- 
void reconectarMQTT() {
  while (!client.connected()) {
    Serial.print("Conectando no MQTT...");
    if (client.connect(mqttClienteId, mqttUsuario, mqttSenha)) {
      Serial.println("conectado!");
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" nova tentativa em 5 segundos");
      delay(5000);
    }
  }
}

// --- MEDIÇÃO DE TEMPERATURA E UMIDADE
void medirTemperaturaUmidade() {
  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimaMedicao >= intervaloMedicao) {
    ultimaMedicao = tempoAtual;
    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature(false);
    client.publish(topicoTemperatura, String(temperatura).c_str(), true);    // Manter a mensagem no BROKER (true or false)
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" *C");
    client.publish(topicoUmidade, String(umidade).c_str(), true);            //  Manter a mensagem no BROKER (true or false)
    Serial.print("Umidade: ");
    Serial.println(umidade);
  }
}
