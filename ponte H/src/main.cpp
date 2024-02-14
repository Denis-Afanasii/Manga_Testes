#include <Arduino.h>

#define AQUECER 5
#define ARREFECER 10

void setup() {

pinMode (AQUECER, OUTPUT);
pinMode (ARREFECER, OUTPUT);

}

void loop() {
digitalWrite(AQUECER, HIGH);
digitalWrite(ARREFECER, LOW);


}

