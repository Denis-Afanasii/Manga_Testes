#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
// #include <DallasTemperature.h>
#include <Ultrasonic.h>


#define ADC_VREF_mV 3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35 25

#define PELTIER 19

unsigned long previousMillisPeltier = 0;
unsigned long previousMillisTemperature = 0;
int interval = 5000;

int peltierState = 0;


WiFiClient espClient;
PubSubClient client(espClient);

OneWire oneWire(26);
// DallasTemperature sensors(&oneWire);

const char *mqtt_broker = "192.168.253.42";
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



  WiFi.begin("pi_pwdam_grupo1", "pwdamBestGrupo");

  Serial.println("A tentar conectar");
  while (WiFi.status() != WL_CONNECTED)
  {
  }
  Serial.println("Connected to WiFi");

  pinMode(PELTIER, OUTPUT);

  client.setServer(mqtt_broker, port);
  Serial.print("Connecting");
  while (!client.connected())
  {
    client.connect(mqtt_clientId.c_str(), mqtt_username, mqtt_password);

    delay(500);
  }
  Serial.print("Connected to MQTT");
  // sensors.begin();
  client.subscribe("ultraSonic");
  // client.setCallback(callback);

  digitalWrite(PELTIER, HIGH);
}

void sendDistance()
{
  const char *topic = "distance";
  Serial.print(ultrasonic1.read());
  const String message = String(ultrasonic1.read());

  Serial.println("Sending message to broker: '" + message + "'.");

  client.publish(topic, String(message).c_str());
}

void sendMacAdress()
{
  const char *topic = "MacAdress";
  const String message = String(WiFi.macAddress());

  Serial.print("Sending message to broker: '" + message + "'.");

  client.publish(topic, String(message).c_str());
}

/*void sendTemperature(){
  const char *topic = "Temperature";
  const String message = String(WiFi.macAddress());

  Serial.print("Sending temperature to broker: '"+ message + "'.");
  client.publish(topic, String(message).c_str);
}*/


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

  int adcVal = analogRead(PIN_LM35);
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);

  /*if (currentMillisPeltier - previousMillisPeltier >= interval && peltierState == 0)
  {
    peltierState = 1;
    previousMillisPeltier = currentMillisPeltier;

    Serial.println("A ligar o Modulo durante 5 segundos");
    digitalWrite(PELTIER, HIGH);
    // delay(20000);
  } else {
    digitalWrite(PELTIER, HIGH);
  }*/

  if (currentMillisTemperature - previousMillisTemperature >= 3000)
  {

    previousMillisTemperature = currentMillisTemperature;

    float tempC = milliVolt / 10;
    Serial.print("Temperatura: ");
    Serial.println(tempC);
  }
  
  sendDistance();

  if (MacSend == 0)
  {
    sendMacAdress();
    Serial.println("Sending Mac Adress");
    MacSend = 1;
  }

  delay(1000);
}
