#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 15
#define DHTTYPE DHT22

const char* ssid = "nome_da_rede";
const char* password = "senha_da_rede";
const char* mqtt_server = "ip_do_home_assistant";
const int mqtt_port = 1883; // Porta padrão do MQTT
const char* mqtt_user = "usuário_MQTT";
const char* mqtt_pass = "senha_MQTT";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32_DHT22", mqtt_user, mqtt_pass)) {
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    client.publish("sensor/dht22/temperature", String(t).c_str(), true);
    client.publish("sensor/dht22/humidity", String(h).c_str(), true);
  }

  delay(5000);
}