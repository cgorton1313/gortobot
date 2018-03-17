#include <Arduino.h>
#include <gb_battery.h>

#ifndef gb_utility_h
#define gb_utility_h

class GbUtility {

private:
GbUtility();
static int32_t EEPROMReadlong(int32_t address);
static void EEPROMWritelong(int16_t address, int32_t value);

public:
static void ClearEEPROM();
static uint16_t IncrementRunNum();
static void GortoNap(uint16_t seconds);
static void WaitForBatteries(uint16_t waitTime, GbBattery battery1, GbBattery battery2);

};

#endif
