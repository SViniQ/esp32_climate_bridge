# 🌡️ ESP32 Climate Bridge

Código feito em C++ para um dispositivo ESP32 com sensor DHT22 (temperatura e umidade) que envia dados via MQTT para estudo e analise. Este código faz parte do projeto SenseTrack.

---

## ⚙️ Funcionalidades

- Leitura de temperatura e umidade com DHT22
- Conexão Wi-Fi automática
- Envio via MQTT para o Home Assistant
- Tópicos MQTT personalizáveis
- Integração com **Home Assistant (via MQTT ou HTTP)**
- Fácil integração com backend externo

---

## 🛠️ Instalação

### 1. Clonar este repositório

```bash
git clone https://github.com/SViniQ/esp32_climate_bridge.git
```

### 2. Abrir o código em um editor de código

- VS Code com a extensão PlatformIO IDE
- Arduino IDE

### 3. Fazer upload do Código na placa ESP32

### 4. Analisar no console se o sensor e a placa ESP32 estão recebendo os cados

---

## 🔌 Esquema de ligação da parte física

Para que o sensor DHT22 funcione corretamente com o ESP32-C3, siga o esquema de ligação abaixo.  
É obrigatório o uso de um **resistor de 10kΩ entre o pino VCC e o pino DATA**.

### 🖼️ Esquema visual:

![Esquema de ligação do DHT22 com ESP32-C3](img\esquema_sensor_e_esp32.png)

> 💡 *Imagem baseada no modelo DHT22 sem placa integrada. Se o seu sensor for “cru”, conecte o resistor de 10kΩ manualmente.*

### 📋 Pinos utilizados:

| DHT22 (4 pinos) | ESP32-C3 | Função                     |
|------------------|------------|----------------------------|
| 1 – VCC          | 3V3        | Alimentação                |
| 2 – DATA         | GPIO4      | Comunicação (com resistor) |
| 3 – NC           | -          | Não conectado              |
| 4 – GND          | GND        | Terra                      |

---

## 💻 Como usar

1. Faça o upload do código via Arduino IDE
2. Instale a biblioteca `DHT sensor library` (by Adafruit)
3. Verifique o monitor serial (115200 baud)
4. Integre com Home Assistant (MQTT ou REST)

---
## 👨🏽‍💻 Feito por:

- Vinícius S. Queiroz - [Link do LinkedIn](https://www.linkedin.com/in/viníciussilvaqueiroz/)

## 🤝 Novas Contribuições?

Sinta-se à vontade para abrir *Issues* ou *Pull Requests*.  
Vamos construir isso juntos e expandir esse projeto!