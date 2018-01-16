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

static void gortoNap(int seconds) {
        for (int i = 0; i < seconds; i++) {
                sleep.sleepDelay(1000);
                delay(DELAY_FOR_SERIAL);
        }
}

static void waitForBatteries(int waitTime) {
        while (!battery1.Okay() && !battery2.Okay()) {
                Serial.print(F("Both batteries critical! Waiting "));
                Serial.print(waitTime);
                Serial.println(F(" seconds."));

                Sleep sleep;
                sleep.pwrDownMode(); // best power saving mode for sleeping

                for (int i = 0; i < waitTime; i++) {
                        sleep.sleepDelay(1000);
                }
                Serial.println(F("Wait time elapsed. Retrying."));
        }
}
