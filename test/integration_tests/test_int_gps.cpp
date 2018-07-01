/* This test exercises the fake GPS */

#include "../../src/navigation/gb_abstract_gps.h"
#include "../../src/utilities/gb_utility.h"
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
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN(F("Gps test starting..."));
}

void loop() {
  GbFix fix = gps.GetFix();
  DEBUG_PRINT(fix.year);
  DEBUG_PRINT(fix.month);
  DEBUG_PRINT(fix.day);
  DEBUG_PRINT("-");
  DEBUG_PRINT(fix.hour);
  DEBUG_PRINT(fix.minute);
  DEBUG_PRINT(fix.second);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(fix.latitude);
  DEBUG_PRINT(", ");
  DEBUG_PRINT(fix.longitude);
  DEBUG_PRINT(", ");
  DEBUG_PRINTLN(fix.satellites);
  delay(500);
}
