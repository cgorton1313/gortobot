#include <Arduino.h>

class GbUtility {

private:
GbUtility();
static long EEPROMReadlong(long address);
static void EEPROMWritelong(int address, long value);

public:
static void ClearEEPROM();
static unsigned int IncrementRunNum();

};
