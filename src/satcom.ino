static void setUpSat() {
        Serial.print(F("Charging super-capacitor. Waiting "));
        Serial.print(SAT_CHARGE_TIME);
        Serial.println(F(" seconds..."));
        delay(DELAY_FOR_SERIAL);
        gortoNap(SAT_CHARGE_TIME); // allow capacitor to charge
        isbd.attachConsole(Serial); // lets me see what the sat modem is doing
        isbd.setPowerProfile(1); // low power profile
        isbd.adjustSendReceiveTimeout(ISBD_TIMEOUT);
        isbd.setMinimumSignalQuality(3); // default is 2, trying this for stability
        isbd.useMSSTMWorkaround(false); // I think I need this here, which is a good thing
}

static void useSat() {
        satOn();
        do {
                char rxBuffer[27]; // max size of rx buffer, was 16
                // expects type(max 1 char),tackAsailPosition(max 3 chars),
                // tackAtime(max 4 chars),tackBsailPosition(max 3 chars),
                // tackBtime(max 4 chars),loggingInterval(maz 4 chars),z
                // = max of 26
                // e.g., "1,359,1439,359,1439,1439,z" is biggest message
                size_t rxBufferSize = sizeof(rxBuffer);
                int error = isbd.sendReceiveSBDText(txBuffer, (uint8_t *)rxBuffer, rxBufferSize);
                if (error != 0) { // there was a problem with send/receive
                        Serial.print(F("sendReceiveSBDText failed: error "));
                        Serial.println(error);
                        txSuccess = false;
                        satOff();
                        return;
                }
                txSuccess = true;
                Serial.print(F("rxBufferSize = "));
                Serial.println(rxBufferSize);
                if (rxBufferSize == 0) {
                        Serial.println(F("No message received"));
                }
                else {
                        Serial.print(F("Message received: "));
                        Serial.println(rxBuffer);
                        parseRxBuffer(rxBuffer);
                        Serial.print(F("Messages left: "));
                        Serial.println(isbd.getWaitingMessageCount());
                }
        } while (isbd.getWaitingMessageCount() > 0);
        satOff();
}

static void satOff() {
        isbd.sleep();
        ISBD_PORT.end();
        blinkMessage(2); // makes sure led doesn't get left ON by mistake
        Serial.println(F("Sat off."));
}

static void satOn() {
        Serial.println(F("Sat on."));
        ISBD_PORT.begin(SAT_BAUD);
        isbd.begin();
}
