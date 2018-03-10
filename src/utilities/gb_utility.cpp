#include <Arduino.h>
#include <EEPROM.h> // for saving the runNum after each re-start
#include "gb_utility.h"

GbUtility::GbUtility() {}

void GbUtility::clearEEPROM() {
        for (unsigned int i = 0; i < EEPROM.length(); i++) {
                EEPROM.write(i, 0);
        }
}

unsigned int GbUtility::incrementRunNum() {
        unsigned int newRunNum;
        newRunNum = EEPROMReadlong(0) + 1;
        EEPROMWritelong(0, newRunNum);
        return newRunNum;
}

long GbUtility::EEPROMReadlong(long address)
{
        //Read the 4 bytes from the eeprom memory.
        long four = EEPROM.read(address);
        long three = EEPROM.read(address + 1);
        long two = EEPROM.read(address + 2);
        long one = EEPROM.read(address + 3);

        //Return the recomposed long by using bitshift.
        return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void GbUtility::EEPROMWritelong(int address, long value)
{
        //Decomposition from a long to 4 bytes by using bitshift.
        //One = Most significant -> Four = Least significant byte
        byte four = (value & 0xFF);
        byte three = ((value >> 8) & 0xFF);
        byte two = ((value >> 16) & 0xFF);
        byte one = ((value >> 24) & 0xFF);

        //Write the 4 bytes into the eeprom memory.
        EEPROM.write(address, four);
        EEPROM.write(address + 1, three);
        EEPROM.write(address + 2, two);
        EEPROM.write(address + 3, one);
}
