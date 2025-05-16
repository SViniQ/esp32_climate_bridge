#include <WiFi.h>           // Library for Wi-Fi connection
#include <PubSubClient.h>   // Library for MQTT communication
#include <DHT.h>            // Library for DHT sensor

#define DHTPIN 33           // Pin where the DHT22 sensor is connected
#define DHTTYPE DHT22       // Define the sensor type as DHT22
DHT dht(DHTPIN, DHTTYPE);   // Instantiate the DHT sensor object

// LED pin definitions for status indication
#define RED_LED_PIN     00  // Red LED: indicates Wi-Fi and sensor read error
#define YELLOW_LED_PIN  00  // Yellow LED: indicates MQTT connection
#define GREEN_LED_PIN   00  // Green LED: indicates successful data send

// Identifiers to simplify LED usage in functions
#define RED     1
#define YELLOW  2
#define GREEN   3

// Wi-Fi and MQTT credentials and configuration
const char* ssid = "";         // Wi-Fi network name
const char* password = "";        // Wi-Fi password
const char* mqtt_server = "";   // MQTT broker address
const int mqtt_port = 1883;                 // MQTT broker port
const char* mqtt_user = "";       // MQTT username
const char* mqtt_pass = "";       // MQTT password

WiFiClient espClient;           // Wi-Fi client
PubSubClient client(espClient); // MQTT client using Wi-Fi

// ----------- Functions ---------------

// Function to blink an LED according to the color and duration specified
void blinkLED(int color, int duration) {
  int pin;
  switch (color) {
    case RED:    pin = RED_LED_PIN; break;
    case YELLOW: pin = YELLOW_LED_PIN; break;
    case GREEN:  pin = GREEN_LED_PIN; break;
    default: return; // Exit if color is not recognized
  }

  digitalWrite(pin, HIGH); // Turn LED on
  delay(duration);         // Wait for the specified time
  digitalWrite(pin, LOW);  // Turn LED off
}

// Function to connect to Wi-Fi, blinking the red LED while connecting
void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Start Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) { // While not connected
    blinkLED(RED, 500);  // Blink red LED every 0.5s
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); // Show obtained IP address
}

// Function to connect to the MQTT broker, blinking yellow LED on failure
void reconnect() {
  while (!client.connected()) { // While not connected to MQTT
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_DHT22", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Error: ");
      Serial.print(client.state()); // Show error code
      Serial.println(" — retrying in 5s");

      blinkLED(YELLOW, 500); // Blink yellow LED
      delay(500);
      blinkLED(YELLOW, 500);
      delay(4000); // Wait 5 seconds before retrying
    }
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600); // Initialize serial communication

  // Initialize LED pins as output and turn them off
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  dht.begin();           // Initialize the DHT sensor
  setup_wifi();          // Connect to Wi-Fi
  client.setServer(mqtt_server, mqtt_port); // Set MQTT server
}

// ---------- Loop ----------
void loop() {
  if (!client.connected()) { // If not connected to MQTT
    reconnect();             // Try to reconnect
  }
  client.loop();             // Keep MQTT connection alive

  float h = dht.readHumidity();    // Read humidity from sensor
  float t = dht.readTemperature(); // Read temperature from sensor

  if (isnan(h) || isnan(t)) { // If reading failed
    Serial.println("Sensor read error!");
    blinkLED(RED, 250);       // Blink red LED quickly
    delay(250);
  } else {
    // Print the values to Serial Monitor
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" °C | Humidity: ");
    Serial.print(h);
    Serial.println(" %");

    // Publish data to MQTT (temperature and humidity topics)
    bool tempSent = client.publish("sensor/dht22/temperature", String(t).c_str(), true);
    bool humSent  = client.publish("sensor/dht22/humidity", String(h).c_str(), true);

    if (tempSent && humSent) { // If both were sent successfully
      Serial.println("Data sent to MQTT!");
      blinkLED(GREEN, 1000);   // Blink green LED for 1 second
    } else {
      Serial.println("Failed to send to MQTT.");
      blinkLED(YELLOW, 300);   // Blink yellow LED quickly
    }
  }

  delay(5000); // Wait 5 seconds between readings
}