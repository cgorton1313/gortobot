/* This test exercises the temp/humidity */

#include "..\lib\Adafruit_HTU21DF_Library\Adafruit_HTU21DF.h"
#include "configs/consts.h"
#include "configs/pins.h"
#include "navigation/gb_fix.h"
#include "navigation/gb_abstract_gps.h"
#include "navigation/gb_gps.h"
#include <Arduino.h>

static Adafruit_HTU21DF htu = Adafruit_HTU21DF();
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);

static GbFix fix;

void setup() {
  Serial.begin(CONSOLE_BAUD);
  Serial.println("HTU21D-F test");
  pinMode(TEMP_HUMIDITY_POWER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, HIGH);

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1)
      ;
  }
}

void loop() {
  Serial.print(htu.readTemperature());
  Serial.print(" | ");
  Serial.println(htu.readHumidity());

  fix = gb_gps.GetFix();
  Serial.print(fix.latitude);
  Serial.print(", ");
  Serial.print(fix.longitude);
  Serial.print(", ");
  Serial.print(fix.hour);
  Serial.print(", ");
  Serial.print(fix.minute);
  Serial.print(", ");
  Serial.println(fix.second);

  delay(500);
}
