#include <Arduino.h>

struct GbFix {
  float latitude;
  float longitude;
  unsigned int year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
};
