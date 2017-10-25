// Utility holds miscellaneous functions


static void clearEEPROM() {
        for (unsigned int i = 0; i < EEPROM.length(); i++) {
                EEPROM.write(i, 0);
        }
}

static void incrementRunNum() {
        runNum = EEPROMReadlong(0) + 1;
        EEPROMWritelong(0, runNum);
}

static void EEPROMWritelong(int address, long value)
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

static long EEPROMReadlong(long address)
{
        //Read the 4 bytes from the eeprom memory.
        long four = EEPROM.read(address);
        long three = EEPROM.read(address + 1);
        long two = EEPROM.read(address + 2);
        long one = EEPROM.read(address + 3);

        //Return the recomposed long by using bitshift.
        return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

static String convertToBase62(unsigned long input) {
        // when decoding the base 62 numbers, remember to look for 5 digits. if this is the case, add a "0"!
        String base62String;
        while ((input / 62) != 0) {
                base62String = BASE_62_CHARACTERS[(input % 62)] + base62String;
                input = input / 62;
        }
        base62String = BASE_62_CHARACTERS[input] + base62String;
        return base62String;
}
