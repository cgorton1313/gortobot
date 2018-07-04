#include "gb_utility.h"
#include <EEPROM.h> // for saving the runNum after each re-start

void GbUtility::ClearEEPROM() {
  DEBUG_PRINTLN(F("Resetting EEPROM"));
  for (uint16_t i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
}

uint16_t GbUtility::IncrementRunNum() {
  uint16_t newRunNum;
  newRunNum = EEPROMReadlong(0) + 1;
  EEPROMWritelong(0, newRunNum);
  return newRunNum;
}

int32_t GbUtility::EEPROMReadlong(int32_t address) {
  // Read the 4 bytes from the eeprom memory.
  int32_t four = EEPROM.read(address);
  int32_t three = EEPROM.read(address + 1);
  int32_t two = EEPROM.read(address + 2);
  int32_t one = EEPROM.read(address + 3);

  // Return the recomposed int32_t by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) +
         ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void GbUtility::EEPROMWritelong(int16_t address, int32_t value) {
  // Decomposition from a int32_t to 4 bytes by using bitshift.
  // One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  // Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}
