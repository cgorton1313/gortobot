#include <Arduino.h>
#include <gb_battery.h>

class GbUtility {

private:
GbUtility();
static long EEPROMReadlong(long address);
static void EEPROMWritelong(int address, long value);

public:
static void ClearEEPROM();
static unsigned int IncrementRunNum();
static void GortoNap(int seconds);
static void WaitForBatteries(int waitTime, GbBattery battery1, GbBattery battery2);

};
