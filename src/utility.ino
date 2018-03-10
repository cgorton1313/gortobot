// Utility holds miscellaneous functions

static void gortoNap(int seconds) {
        delay(DELAY_FOR_SERIAL);
        for (int i = 0; i < seconds; i++) {
                sleep.sleepDelay(1000);
        }
}

static void waitForBatteries(int waitTime) {
        char battery1Status = battery1.Status();
        char battery2Status = battery2.Status();
        boolean batteriesCritical = (battery1Status == 'r' && battery2Status == 'r');

        if (batteriesCritical) {
            Serial.println(F("Both batteries critical!"));
            while (battery1Status != 'g' && battery2Status != 'g') {
                    Serial.print(F("Neither battery green. Waiting "));
                    Serial.print(waitTime);
                    Serial.println(F(" seconds."));

                    for (int i = 0; i < waitTime; i++) {
                            gortoNap(1);
                    }
                    Serial.println(F("Wait time elapsed. Retrying."));
                    battery1Status = battery1.Status();
                    battery2Status = battery2.Status();
            }
        }
}
