//#include <Arduino.h>
#include <stdint.h>

#ifndef GB_FIX_H
#define GB_FIX_H

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
