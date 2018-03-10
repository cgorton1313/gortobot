#include <Arduino.h>

class GbUtility {

private:
GbUtility();
static long EEPROMReadlong(long address);
static void EEPROMWritelong(int address, long value);

public:
static void clearEEPROM();
static unsigned int incrementRunNum();

};
