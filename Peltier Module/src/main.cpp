#include <Arduino.h>

/*
//int buttonPin = 21;
#define buttonPin 21;
int buttonInput = 1;
int buttonState = 0;
//int peltier = 25;
#define peltier 25;

void setup()
{

    Serial.begin(9600);
    pinMode(21, INPUT);
}

void loop()
{
    buttonInput = digitalRead(21);

    if (buttonInput == 0)
    {
        if (buttonState == 0)
        {
            buttonState = 1;
            Serial.print("Ligado");
        }
        else
        {
            buttonState = 0;
            Serial.print("Desligado");
        }

        delay(500);
    }

    if (buttonState == 1)
    {

        digitalWrite(25, HIGH); // Look first on how much voltage your peltier unit can handle 255 = 5V
    }
    else
    {

        digitalWrite(25, LOW);
    }

    delay(500);
}*/


const int BUTTON_PIN = 21; // the number of the pushbutton pin

// Variables will change:
int lastState = HIGH; // the previous state from the input pin
int currentState;    // the current reading from the input pin

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize the pushbutton pin as an pull-up input
  // the pull-up input pin will be HIGH when the switch is open and LOW when the switch is closed.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == LOW && currentState == HIGH){
    Serial.println("The state changed from LOW to HIGH");
  }
  // save the last state
  lastState = currentState;
}