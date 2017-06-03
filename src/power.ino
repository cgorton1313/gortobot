// power.ino handles solar, battery, power reporting, sleeping


static void gortoNap(int seconds) {
        for (int i = 0; i < seconds; i++) {
                Narcoleptic.delay(1000);
                delay(delayForSerial);
        }
}

static void processVoltageData() {
        if (!checkingVoltage) {
                Serial.println(F("Skipping voltage check. Faking voltage at 3.99."));
                batteryVoltage = 3.99; // fake the battery voltage
        }
        else { // wait for enough voltage
                getVolts();
                if (batteryVoltage < minBatteryVoltage) {
                        batteryCritical = true;
                }
                while (batteryCritical) {
                        Serial.print(F("Battery critical! Volts = "));
                        Serial.print(batteryVoltage);
                        Serial.print(F(". Waiting "));
                        Serial.print(batteryWaitTime);
                        Serial.println(F(" seconds."));
                        delay(delayForSerial);
                        gortoNap(batteryWaitTime);
                        getVolts();
                        if (batteryVoltage > batteryOkayVoltage) {
                                batteryCritical = false;
                        }
                }
                Serial.print(F("Battery okay. Volts = "));
                Serial.println(batteryVoltage);
                delay(delayForSerial);
        }
}

static void getVolts() {
        int batteryVoltageInt = 0;
        float correction = 1;
        const int samples = 10; // number of samples to take
        for (int i = 0; i < samples; i++) {
                delay(5);
                batteryVoltageInt = batteryVoltageInt + analogRead(batteryVoltagePin);
        }
        batteryVoltage = 5.08 * correction * (((float)batteryVoltageInt / samples) / 1023.0);
}
