#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

#define ADC_VREF_mV 3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35 32
#define PELTIER 19
#define LONG_RANGE
// #define HIGH_SPEED
// #define HIGH_ACCURACY

// Delays
unsigned long previousMillisPeltier = 0;
unsigned long previousMillisTemperature = 0;
unsigned long previousMillisVolume = 0;
int interval = 5000;
int peltierState = 0;

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_broker = "192.168.50.10";
const char *mqtt_username = "pwdam";
const char *mqtt_password = "pwdam";
const String mqtt_clientId = "esp32-client";
const uint16_t port = 1883;

const float upperRadius = 8 / 2; // Raio da parte superior do copo em centímetros
const float lowerRadius = 4 / 2; // Raio da parte inferior do copo em centímetros
const float containerHeight = 8; // Altura total do copo em centímetros

// Calcular o Volume
float calculateVolume(float distanceInCentimeters)
{
  float height = containerHeight - distanceInCentimeters;

  float volumeInLiters = (((PI * height) / 3) * (pow(upperRadius, 2) + pow(lowerRadius, 2) + (upperRadius * lowerRadius)) / 1000);

  return volumeInLiters;
}

// Mandar o Volume
void sendVolume(float distance)
{

  const char *topic = "volume";

  String message = "{\"clientId\": \"" + WiFi.macAddress() + "\", \"message\": \"" + calculateVolume(distance) + "\"}";

  Serial.println("Sending Volume: '" + message + "'.");

  client.publish(topic, String(message).c_str());

  //  {"clientId":"","message":""}
}

// Mandar o MacAdress
void sendMacAdress()
{
  const char *topic = "macadress";
  const String message = String(WiFi.macAddress());

  Serial.println("Sending MacAdress: '" + message + "'.");
  client.publish(topic, String(message).c_str());
}

// Mandar a Temperatura
void sendTemp()
{
  int adcVal = analogRead(PIN_LM35);
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);

  float tempC = milliVolt / 10;

  const char *topic = "temp";
  String message = "{\"clientId\": \"" + WiFi.macAddress() + "\", \"message\": \"" + tempC + "\"}";

  Serial.println("Sending Temperature: '" + message + "'.");

  client.publish(topic, String(message).c_str());
}

// Processar e filtrar a mensagem recebida
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensagem recebida no seguinte tópico: ");
  Serial.print(topic);

  Serial.print(" | Mensagem: '");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("' |");

  if (String(topic) == "peltierControl")
  {
    String message = "";
    for (int i = 0; i < length; i++)
    {
      message.concat((char)payload[i]);
    }

    Serial.println(message);

    if (message == "ON")
    {
      digitalWrite(PELTIER, HIGH);
      peltierState = 1;
    }
    else if (message == "OFF") // if (String((char)payload[0]) == "OFF")
    {
      digitalWrite(PELTIER, LOW);
      peltierState = 0;
    }
  }
  else if (String(topic) == "askTemp")
  {
    String message = "";
    for (int i = 0; i < length; i++)
    {
      message.concat((char)payload[i]);
    }
    Serial.println(message);
    if (message == "TEMP")
    {
      sendTemp();
    }
  }
  else if (String(topic) == "askVolume")
  {
    String message = "";
    for (int i = 0; i < length; i++)
    {
      message.concat((char)payload[i]);
    }
    Serial.println(message);
    if (message == "Volume")
    {
      sendTemp();
    }
  }
}

void setup()
{
  // PORTA SERIAL
  Serial.begin(9600);
  Serial.println("\nConsole started.");
  // WIFI
  WiFi.begin("RPiHotspot", "1234567890");
  Serial.println("A tentar conectar ao WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
  }
  Serial.println("Connected to WiFi");

  // VL53L0X
  Wire.begin(4, 0);
  sensor.setTimeout(1500);
  if (!sensor.init(false))
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1)
    {
    }
  }

#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  sensor.setMeasurementTimingBudget(500000);
#endif

  // PELTIER
  pinMode(PELTIER, OUTPUT);

  client.setServer(mqtt_broker, port);
  Serial.println("Connecting");
  while (!client.connected())
  {
    client.connect(mqtt_clientId.c_str(), mqtt_username, mqtt_password);
    delay(500);
  }
  Serial.println("Connected to MQTT");
  client.subscribe("distanceSensor");
  client.subscribe("peltierControl");

  client.setCallback(callback);

  //  digitalWrite(PELTIER, HIGH);
}

void loop()
{
  client.loop();

  float distancia = (sensor.readRangeSingleMillimeters() - 40) / 10;
  /*Serial.print(distancia);
  Serial.println("cm");*/

  if (sensor.timeoutOccurred())
  {
    Serial.print("Sensor de Distancia TIMEOUT");
  }

  unsigned long currentMillisPeltier = millis();
  unsigned long currentMillisTemperature = millis();
  unsigned long currentMillisVolume = millis();

  if (currentMillisTemperature - previousMillisTemperature >= 3000)
  {
    previousMillisTemperature = currentMillisTemperature;

    sendTemp();
  }

  if (currentMillisVolume - previousMillisVolume >= 10000)
  {
    previousMillisVolume = currentMillisVolume;

    sendVolume(distancia);
  }
}
