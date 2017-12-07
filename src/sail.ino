// sail.ino handles the sails (trimming, reporting, etc.)

unsigned long howLongWatchShouldBe() {
        if (txSuccess) {
                return loggingInterval; // in seconds, need the UL for large times
        }
        else {
                return min(FAILURE_RETRY, loggingInterval); // in seconds
        }
}

static void useSail() {
        switch (sailMode) {
        case 'r': // real sailing
                realSail();
                break;
        case 't': // testing sail
                Serial.println(F("Entering sail test mode."));
                testSail();
                break;
        case 's': // direct sets
                Serial.println(F("Entering direct set sail mode."));
                directSetSail();
                break;
        }
}

static void realSail() {
        Serial.print(F("Sailing for "));
        Serial.print(thisWatch);
        Serial.println(F(" seconds."));
        delay(DELAY_FOR_SERIAL);
        unsigned long timer = 0; // used to track seconds during sail operation
        while (timer < thisWatch) {
                if ((timer % 60) == 0) { // check trim every 60 seconds
                        Serial.println(F("At :00 seconds now"));
                        if ((tackIsA && currentTackTime >= orderedTackTimeA) || (!tackIsA && currentTackTime >= orderedTackTimeB)) {
                                tackIsA = !tackIsA; // change tacks
                                currentTackTime = 0;
                                Serial.println(F("Changing tacks"));
                        }
                        if (tackIsA) {
                                orderedSailPosition = orderedSailPositionA;
                                Serial.println(F("Sailing on tack A"));
                        } else {
                                orderedSailPosition = orderedSailPositionB;
                                Serial.println(F("Sailing on tack B"));
                        }
                        if (validOrders(orderedSailPosition)) {
                                trimSail(orderedSailPosition);
                        }
                        currentTackTime++;
                        Serial.print(F("Current tack time = "));
                        Serial.println(currentTackTime);
                        delay(DELAY_FOR_SERIAL);
                }
                gortoNap(6); // six seconds of napping
                timer = timer + 6;
                blinkMessage(2); // flash led
                delay(DELAY_FOR_SERIAL);
                Serial.print(F("timer = "));
                Serial.println(timer);
                delay(DELAY_FOR_SERIAL);
        }
}

static void testSail() {
        //int testSailPositions[] = {330, 270, 225, 210, 150, 135, 90, 30};
        int testSailPositions[] = {0,30,60,90,120,150,180,210,240,270,300,330,360};
        Serial.print(F("Testing sail."));
        for (byte i = 0; i < (sizeof(testSailPositions) / sizeof(int)); i++) {
                orderedSailPosition = testSailPositions[i];
                setTestSailPosition(orderedSailPosition);
        }
}

static void directSetSail() {
        setTestSailPosition(orderedSailPosition);
}

static boolean validOrders(int order) {
        if ((order > MAXIMUM_SAIL_ANGLE) || (order < MINIMUM_SAIL_ANGLE)) {
                return false; // out-of-bouds
        }
        else {
                return true;
        }
}

static void trimSail(int orderedSailPosition) {
        unsigned long trimTimer;
        int trimSeconds = 0;
        int totalTrimSeconds = 0;
        boolean sailIsTrimming = true;
        processVoltageData();
        sailPosition = sail.GetPosition();
        int tempSailPosition = sailPosition;
        trimRoutineExceededMax = false;
        sailNotMoving = false;

        if ((abs(sailPosition - orderedSailPosition)) > 4) { // sail is out of trim
                trimTimer = millis();  // capture the time now
                while (((sailPosition - orderedSailPosition) != 0) && sailIsTrimming) {
                        // trim sail
                        digitalWrite(MOTOR_IN_1_PIN, sailPosition > orderedSailPosition);
                        digitalWrite(MOTOR_IN_2_PIN, sailPosition < orderedSailPosition);
                        if ((unsigned long)(millis() - trimTimer) >= 1000) { // 1 second has passed
                                trimTimer = millis(); // reset second timer
                                totalTrimSeconds = totalTrimSeconds + 1;
                                if (totalTrimSeconds > TRIM_ROUTINE_MAXIMUM_SECONDS) {
                                        sailIsTrimming = false;
                                        trimRoutineExceededMax = true;
                                        Serial.println(F("trimRoutineExceededMax"));
                                }
                                sailPosition = sail.GetPosition();
                                if (sailPosition == tempSailPosition) {
                                        // sail hasn't moved in the last second
                                        trimSeconds = trimSeconds + 1;
                                        if (trimSeconds > 10) {
                                                //sail hasn't moved in 10 seconds, bad!
                                                sailIsTrimming = false;
                                                sailNotMoving = true;
                                                Serial.println(F("Sail not moving"));
                                        }
                                }
                                else { // sail has moved
                                        tempSailPosition = sailPosition;
                                        trimSeconds = 0;
                                        sailNotMoving = false;
                                }
                        }
                }
                stopMotors(); // sail is either in position or stuck
        }
        Serial.print(F("totalTrimSeconds = "));
        Serial.println(totalTrimSeconds);
        delay(DELAY_FOR_SERIAL);
}

static void stopMotors() {
        digitalWrite(MOTOR_IN_1_PIN, LOW);
        digitalWrite(MOTOR_IN_2_PIN, LOW);
}

static void setTestSailPosition(int thePosition) {
        trimSail(thePosition);
        unsigned long testTimer = 0;
        if (sailMode != 's') {
                while (testTimer < loggingInterval) { // so the duration can be set via RX
                        gortoNap(1); // one second of napping
                        delay(DELAY_FOR_SERIAL);
                        testTimer = testTimer + 1;
                        blinkMessage(2); // flash led
                        Serial.print(F("timer = "));
                        Serial.println(testTimer);
                        delay(DELAY_FOR_SERIAL);
                }
        }
}

static void pretendSail() {
        delay(DELAY_FOR_SERIAL);
        Serial.print(F("Pretending to sail for "));
        Serial.print(thisWatch);
        Serial.println(F(" seconds."));
        delay(DELAY_FOR_SERIAL);
        unsigned long timer = 0; // used to track seconds during sail operation
        while (timer < thisWatch) {
                gortoNap(1); // one second of napping
                timer = timer + 1;
                blinkMessage(2); // flash led
                delay(DELAY_FOR_SERIAL);
                Serial.print(F("timer = "));
                delay(DELAY_FOR_SERIAL);
                Serial.println(timer);
                delay(10);
        }
}
