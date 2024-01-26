#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

#define LONG_RANGE

//#define HIGH_SPEED
//#define HIGH_ACCURACY

void setup(){
Serial.begin(9600);
Wire.begin(4, 0);


  sensor.setTimeout(1500);
  if (!sensor.init(false))
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
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


}

void loop(){
  float distancia = sensor.readRangeSingleMillimeters() - 40;
  Serial.print(distancia);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
}
