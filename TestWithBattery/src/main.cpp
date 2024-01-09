#include <Arduino.h>

#define ADC_VREF_mV 3300.0
#define ADC_RESOLUTION 4096.0
#define PIN_LM35 25

#define PELTIER 19

unsigned long previousMillisPeltier = 0;
unsigned long previousMillisTemperature = 0;
int interval = 5000;

int peltierState = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(PELTIER, OUTPUT);
}

void loop()
{

  /*Serial.println("Millis atual:");
  Serial.print(millis());*/
  unsigned long currentMillisPeltier = millis();
  unsigned long currentMillisTemperature = millis();

  int adcVal = analogRead(PIN_LM35);
  float milliVolt = adcVal * (ADC_VREF_mV / ADC_RESOLUTION);

  /* float tempC = milliVolt / 10;
  Serial.print("Temperatura: ");
  Serial.print(tempC);
  delay(5000); */

  if (currentMillisPeltier - previousMillisPeltier >= interval && peltierState == 0)
  {
    peltierState = 1;
    previousMillisPeltier = currentMillisPeltier;

    Serial.println("A ligar o Modulo durante 5 segundos");
    digitalWrite(PELTIER, HIGH);
    // delay(20000);
  } else {
    digitalWrite(PELTIER, HIGH);
  }

  if (currentMillisTemperature - previousMillisTemperature >= 3000)
  {

    previousMillisTemperature = currentMillisTemperature;

    float tempC = milliVolt / 10;
    Serial.print("Temperatura: ");
    Serial.println(tempC);
  }
}
