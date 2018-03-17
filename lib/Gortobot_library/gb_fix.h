#include <Arduino.h>

#ifndef gb_fix_h
#define gb_fix_h

struct GbFix {
  float latitude;
  float longitude;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t satellites;
};

#endif
