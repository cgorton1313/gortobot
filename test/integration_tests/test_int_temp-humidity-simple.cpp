/* This test exercises the temp/humidity */

#include "..\..\lib\Adafruit_HTU21DF_Library\Adafruit_HTU21DF.h"
#include "../../src/configs/consts.h"
#include "../../src/configs/pins.h"
#include "../../src/configs/config.h"
#include "../../src/navigation/gb_fix.h"
#include "../../src/navigation/gb_abstract_gps.h"
#include "../../src/navigation/gb_gps.h"
#include <Arduino.h>

static Adafruit_HTU21DF htu = Adafruit_HTU21DF();
static GbGps gb_gps =
    GbGps(GPS_POWER_PIN_1, GPS_POWER_PIN_2, GPS_SERIAL_PORT, GPS_BAUD);

static GbFix fix;

void setup() {
  DEBUG_BEGIN(CONSOLE_BAUD);
  DEBUG_PRINTLN("HTU21D-F test");
  pinMode(TEMP_HUMIDITY_POWER_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(TEMP_HUMIDITY_POWER_PIN, HIGH);

  if (!htu.begin()) {
    DEBUG_PRINTLN("Couldn't find sensor!");
    while (1)
      ;
  }
}

void loop() {
  DEBUG_PRINT(htu.readTemperature());
  DEBUG_PRINT(" | ");
  DEBUG_PRINTLN(htu.readHumidity());

  fix = gb_gps.GetFix();
  DEBUG_PRINT(fix.latitude);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(fix.longitude);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(fix.hour);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(fix.minute);
  DEBUG_PRINT(", ");
  DEBUG_PRINTLN(fix.second);

  delay(500);
}
