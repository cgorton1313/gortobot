/* This test exercises the fake GPS */

#include "..\lib\Adafruit_HTU21DF_Library\Adafruit_HTU21DF.h"
#include "configs/pins.h"
#include <Arduino.h>

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

void setup() {
  Serial.begin(115200);
  Serial.println("HTU21D-F test");
  pinMode(TEMP_HUMIDITY_POWER_PIN, OUTPUT);
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, HIGH);

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
}

void loop() {
  Serial.print("Temp: "); Serial.print(htu.readTemperature());
  Serial.print("\t\tHum: "); Serial.println(htu.readHumidity());
  delay(500);
}
