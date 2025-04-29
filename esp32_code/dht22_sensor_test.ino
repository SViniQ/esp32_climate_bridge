#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22  


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();       
}

void loop() {
  float humidity = dht.readHumidity();

  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Falha na leitura do sensor DHT22!");
    delay(2000);
    return;
  }
  
  Serial.print("Temperatura: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Umidade: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(2000);
}