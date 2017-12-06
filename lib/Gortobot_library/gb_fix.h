#include <Arduino.h>

#ifndef gb_fix_h
#define gb_fix_h

struct GbFix {
  float latitude;
  float longitude;
  unsigned int year;
  byte month;
  byte day;
  byte hour;
  byte minute;
  byte second;
  byte satellites;
};

#endif
