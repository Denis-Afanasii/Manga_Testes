#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(9600);
  Wire.begin(4, 0);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  delay(1000);

  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin(0x29, true, &Wire)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }









  // power
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));

  // start continuous ranging
  lox.startRangeContinuous();
}

void loop() {
  if (lox.isRangeComplete()) {
    Serial.print("Distance in mm: ");
    Serial.println(lox.readRange());
  }
}