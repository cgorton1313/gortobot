/* This test exercises the fake GPS */

#include "navigation/gb_abstract_gps.h"
#include "utilities/gb_utility.h"
#include <Arduino.h>

class FakeGps : public GbAbstractGps {
private:
  char _scheme;

public:
  FakeGps(char scheme)
      : GbAbstractGps(), _scheme(scheme){};

  GbFix GetFix() {
    GbFix fix;
    fix.latitude = random(-800000, 800000) / 10000.0;
    fix.longitude = random(-1800000, 1800000) / 10000.0;
    fix.year = 2016 + random(5); // any year b/w 2016 and 2020
    fix.month = 1 + random(12);
    fix.day = 1 + random(31); // will sometimes give bogus dates
    fix.hour = 1 + random(23);
    fix.minute = 1 + random(59);
    fix.second = 1 + random(59);
    fix.satellites = 4;
    return fix;
  }
};

FakeGps gps = FakeGps('s');

void setup() {
  Serial.begin(115200);
  Serial.println(F("Gps test starting..."));
}

void loop() {
  GbFix fix = gps.GetFix();
  Serial.print(fix.year);
  Serial.print(fix.month);
  Serial.print(fix.day);
  Serial.print("-");
  Serial.print(fix.hour);
  Serial.print(fix.minute);
  Serial.print(fix.second);
  Serial.print(", ");
  Serial.print(fix.latitude);
  Serial.print(", ");
  Serial.print(fix.longitude);
  Serial.print(", ");
  Serial.println(fix.satellites);
  delay(500);
}
