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

/*void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.print(topic);

  Serial.print(" | Message: '");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("'");

  if (String(topic) == "ultraSonic")
  {
    String message = "";
    for(int i = 0; i < length; i++){
      message.concat((char)payload[i]);
    }

    Serial.print(message);

    if (message == "ON")
    {
      Serial.print("Turning On UltraSonic Sensor!");
      analogWrite(21, 255);
      analogWrite(19, 255);
    }
    else //if (String((char)payload[0]) == "OFF")
    {
      analogWrite(21, );
      analogWrite(19, 0);
    }
  }
} */

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
  // client.setCallback(callback);

  digitalWrite(PELTIER, HIGH);
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



  if (currentMillisTemperature - previousMillisTemperature >= 3000)
  {
    previousMillisTemperature = currentMillisTemperature;

    sendTemp();
  }

  if (currentMillisDistance - previousMillisDistance >= 10000)
  {
    previousMillisDistance = currentMillisDistance;

    sendDistance();
  }

  ;

  if (MacSend == 0)
  {
    sendMacAdress();
    MacSend = 1;
  }
}
