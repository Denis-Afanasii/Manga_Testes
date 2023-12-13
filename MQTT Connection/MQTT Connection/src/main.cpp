#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
//#include <DallasTemperature.h>
#include <Ultrasonic.h>

WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(26);
//DallasTemperature sensors(&oneWire);

const char *mqtt_broker = "192.168.137.5";
const char *mqtt_username = "pwdam";
const char *mqtt_password = "pwdam";
const String mqtt_clientId = "esp32-client";
const uint16_t port = 1883;

Ultrasonic ultrasonic1(21, 19);

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

  if (WiFi.begin("Ryzen's Laptop", "Amogus#4"))
  {
    Serial.println("Connected to wifi!");
  }

  client.setServer(mqtt_broker, port);
  Serial.print("Connecting");
  while (!client.connected())
  {
    client.connect(mqtt_clientId.c_str(), mqtt_username, mqtt_password);
    
    delay(500);
  }
  Serial.print("Connected to MQTT");
  //sensors.begin();
  client.subscribe("ultraSonic");
  //client.setCallback(callback);
}

void sendDistance()
{
    const char *topic = "distance";
    Serial.print(ultrasonic1.read());
    const String message = String(ultrasonic1.read());


    Serial.println("Sending message to broker: '" + message + "'.");

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

  //sendTemp();
sendDistance();
  delay(1000);
}
