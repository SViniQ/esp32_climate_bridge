#include <WiFi.h>           // Biblioteca para conexão Wi-Fi
#include <PubSubClient.h>   // Biblioteca para comunicação MQTT
#include <DHT.h>            // Biblioteca para o sensor DHT

#define DHTPIN 33           // Pino onde o sensor DHT22 está conectado
#define DHTTYPE DHT22       // Define o tipo do sensor como DHT22
DHT dht(DHTPIN, DHTTYPE);   // Instancia o objeto do sensor DHT

// Definições dos pinos dos LEDs de status
#define RED_LED_PIN     00  // LED vermelho: indica Wi-Fi e erro de leitura
#define YELLOW_LED_PIN  00  // LED amarelo: indica conexão MQTT
#define GREEN_LED_PIN   00  // LED verde: indica envio com sucesso

// Identificadores para facilitar o uso dos LEDs nas funções
#define RED     1
#define YELLOW  2
#define GREEN   3

// Credenciais e configurações de rede Wi-Fi e MQTT
const char* ssid = "";         // Nome da rede Wi-Fi
const char* password = "";        // Senha da rede Wi-Fi
const char* mqtt_server = "";   // Endereço do broker MQTT
const int mqtt_port = 1883;                 // Porta do broker MQTT
const char* mqtt_user = "";       // Usuário MQTT
const char* mqtt_pass = "";       // Senha MQTT

WiFiClient espClient;           // Cliente Wi-Fi
PubSubClient client(espClient); // Cliente MQTT usando o Wi-Fi

// ----------- Funções ---------------

// Função para piscar um LED de acordo com a cor e duração especificadas
void blinkLED(int color, int duration) {
  int pin;
  switch (color) {
    case RED:    pin = RED_LED_PIN; break;
    case YELLOW: pin = YELLOW_LED_PIN; break;
    case GREEN:  pin = GREEN_LED_PIN; break;
    default: return; // Sai se a cor não for reconhecida
  }

  digitalWrite(pin, HIGH); // Liga o LED
  delay(duration);         // Aguarda o tempo especificado
  digitalWrite(pin, LOW);  // Desliga o LED
}

// Função para conectar ao Wi-Fi, piscando o LED vermelho enquanto conecta
void setup_wifi() {
  Serial.println();
  Serial.print("Conectando-se à rede Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Inicia a conexão Wi-Fi
  while (WiFi.status() != WL_CONNECTED) { // Enquanto não conectar
    blinkLED(RED, 500);  // Pisca o LED vermelho a cada 0,5s
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Mostra o IP obtido
}

// Função para conectar ao broker MQTT, piscando o LED amarelo em caso de falha
void reconnect() {
  while (!client.connected()) { // Enquanto não estiver conectado ao MQTT
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP32_DHT22", mqtt_user, mqtt_pass)) {
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.print("Erro: ");
      Serial.print(client.state()); // Mostra o código de erro
      Serial.println(" — tentando novamente em 5s");

      blinkLED(YELLOW, 500); // Pisca o LED amarelo
      delay(500);
      blinkLED(YELLOW, 500);
      delay(4000); // Aguarda 5 segundos antes de tentar novamente
    }
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial

  // Inicializa os pinos dos LEDs como saída e os apaga
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  dht.begin();           // Inicializa o sensor DHT
  setup_wifi();          // Conecta ao Wi-Fi
  client.setServer(mqtt_server, mqtt_port); // Configura o servidor MQTT
}

// ---------- Loop ----------
void loop() {
  if (!client.connected()) { // Se não estiver conectado ao MQTT
    reconnect();             // Tenta reconectar
  }
  client.loop();             // Mantém a conexão MQTT ativa

  float h = dht.readHumidity();    // Lê a umidade do sensor
  float t = dht.readTemperature(); // Lê a temperatura do sensor

  if (isnan(h) || isnan(t)) { // Se a leitura falhar
    Serial.println("Erro ao ler sensor!");
    blinkLED(RED, 250);       // Pisca o LED vermelho rapidamente
    delay(250);
  } else {
    // Mostra os valores lidos no Serial Monitor
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" °C | Umidade: ");
    Serial.print(h);
    Serial.println(" %");

    // Publica os dados no MQTT (tópicos de temperatura e umidade)
    bool tempSent = client.publish("sensor/dht22/temperature", String(t).c_str(), true);
    bool humSent  = client.publish("sensor/dht22/humidity", String(h).c_str(), true);

    if (tempSent && humSent) { // Se ambos foram enviados com sucesso
      Serial.println("Dados enviados ao MQTT!");
      blinkLED(GREEN, 1000);   // Pisca o LED verde por 1 segundo
    } else {
      Serial.println("Falha ao enviar ao MQTT.");
      blinkLED(YELLOW, 300);   // Pisca o LED amarelo rapidamente
    }
  }

  delay(5000); // Aguarda 5 segundos entre leituras
}