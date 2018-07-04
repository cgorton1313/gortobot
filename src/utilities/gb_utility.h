/*
Utility class for methods that don't belong anywhere else.
*/

#include <Arduino.h>
#include "../power/gb_abstract_battery.h"
#include "../../src/configs/config.h"
#include <stdint.h>

#ifndef GB_UTILITY_H
#define GB_UTILITY_H

class GbUtility {

private:
  GbUtility(){};
  static int32_t EEPROMReadlong(int32_t address);
  static void EEPROMWritelong(int16_t address, int32_t value);

public:
  static void ClearEEPROM();
  static uint16_t IncrementRunNum();
  static void GortoNap(uint16_t seconds);
  static void WaitForBatteries(uint16_t waitTime, GbAbstractBattery &battery1,
                               GbAbstractBattery &battery2);
};

#endif
