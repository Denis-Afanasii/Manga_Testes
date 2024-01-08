#include <Arduino.h>

#define LED 19

unsigned long previousMillis = 0;
const long interval = 100;


float tempc;
float tempf;
float vout;


void setup() {
Serial.begin(9600);
pinMode(LED, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

unsigned long currentMillis = millis();

if(currentMillis - previousMillis >= interval){

previousMillis = currentMillis;


  Serial.println("A aquecer o Modulo durante 20 segundos");
  digitalWrite(LED, HIGH);
  //delay(20000);
  }
  
  Serial.println("A desligar o Módulo durante 20 segundos");
  digitalWrite(LED, LOW);
  delay(20000);

}
