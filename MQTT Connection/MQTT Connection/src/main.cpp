#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
// #include <DallasTemperature.h>
#include <Ultrasonic.h>

#define ADC_VREF_mV 3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35 32
#define PELTIER 19

unsigned long previousMillisPeltier = 0;
unsigned long previousMillisTemperature = 0;
unsigned long previousMillisDistance = 0;
int interval = 5000;
int peltierState = 0;

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(26);
// DallasTemperature sensors(&oneWire);

const char *mqtt_broker = "192.168.50.10";
const char *mqtt_username = "pwdam";
const char *mqtt_password = "pwdam";
const String mqtt_clientId = "esp32-client";
const uint16_t port = 1883;
int MacSend = 0;

Ultrasonic ultrasonic1(21, 22);

const float containerVolume = 180.0;
const float containerHeight = 8.0;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.print(topic);

  Serial.print(" | Message: '");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("'");

  if (String(topic) == "peltierControl")
  {
    String message = "";
    for (int i = 0; i < length; i++)
    {
      message.concat((char)payload[i]);
    }

    Serial.print(message);

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
}

void setup()
{
  Serial.begin(9600);
  Serial.println("\nConsole started.");

  WiFi.begin("RPiHotspot", "1234567890");

  Serial.println("A tentar conectar");
  while (WiFi.status() != WL_CONNECTED)
  {
  }
  Serial.println("Connected to WiFi");

  pinMode(PELTIER, OUTPUT);

  client.setServer(mqtt_broker, port);
  Serial.println("Connecting");
  while (!client.connected())
  {
    client.connect(mqtt_clientId.c_str(), mqtt_username, mqtt_password);

    delay(500);
  }
  Serial.println("Connected to MQTT");
  // sensors.begin();
  client.subscribe("ultraSonic");
  client.subscribe("peltierControl");
  client.setCallback(callback);

  //  digitalWrite(PELTIER, HIGH);
}

float calculateVolume(float distanceInCentimeters)
{
  const float containerHeight = 300.0;

  float distanceInMeters = distanceInCentimeters / 100.0;

  // Calcule o raio com base no volume total e altura
  float radius = sqrt(containerVolume / (PI * containerHeight));

  // Calcule o volume do cilindro usando o raio e a distância medida pelo sensor até a superfície do líquido
  float volumeInLiters = PI * pow(radius, 2) * (containerHeight - distanceInMeters) / 1000.0;

  return volumeInLiters;
}

void sendVolume()
{

  const char *topic = "volume";

  String message = "{\"clientId\": \"" + WiFi.macAddress() + "\", \"message\": \"" + calculateVolume(ultrasonic1.read()) +"\"}";

  Serial.println("Sending Volume: '" + message + "'.");

  client.publish(topic, String(message).c_str());

  //  {"clientId":"","message":""}
}

void sendDistance()
{
  const char *topic = "distance";
  String message = "{\"clientId\": \"" + WiFi.macAddress() + "\", \"message\": \"" + ultrasonic1.read() + "\"}";

  Serial.println("Sending Distance: '" + message + "'.");

  client.publish(topic, String(message).c_str());

  //  {"clientId":"","message":""}
}

void sendMacAdress()
{
  const char *topic = "MacAdress";
  const String message = String(WiFi.macAddress());

  Serial.println("Sending MacAdress: '" + message + "'.");
  client.publish(topic, String(message).c_str());
}

void sendTemp()
{
  int adcVal = analogRead(PIN_LM35);
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);

  float tempC = milliVolt / 10;

  const char *topic = "Temperature";
  String message = "{\"clientId\": \"" + WiFi.macAddress() + "\", \"message\": \"" + tempC + "\"}";

  Serial.println("Sending Temperature: '" + message + "'.");

  client.publish(topic, String(message).c_str());
}

/*void sendTemp()
{
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

  const char *topic = "temp";
  // const String message = String(temperatureC) + "C / " + String(temperatureF) + "F";
  const String message = String(temperatureC);

  Serial.println("Sending message to broker: '" + message + "'.");

  client.publish(topic, String(message).c_str());
}*/

void loop()
{
  client.loop();

  /*const char *topic = "raspberry_mqtt";
  const String message = "test";

  client.publish(topic, String(message).c_str());*/

  // sendTemp();
  unsigned long currentMillisPeltier = millis();
  unsigned long currentMillisTemperature = millis();
  unsigned long currentMillisDistance = millis();

  /*if (currentMillisTemperature - previousMillisTemperature >= 3000)
  {
    previousMillisTemperature = currentMillisTemperature;

    sendTemp();
  }

  if (currentMillisDistance - previousMillisDistance >= 10000)
  {
    previousMillisDistance = currentMillisDistance;

    sendDistance();
    sendVolume();
  }*/

  float distance = ultrasonic1.read();
  Serial.println("Distance: " + String(distance) + " cm");
  float fixedDistance = 10.0;
  float volume = calculateVolume(fixedDistance);
  Serial.println("Volume: " + String(volume) + " liters");

  if (MacSend == 0)
  {
    sendMacAdress();
    MacSend = 1;
  }
  delay(1000);
}
