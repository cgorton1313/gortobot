// communication.ino handles creation of the log sentence,
//  satellite/wifi TX and RX, etc.

// MT message starts with message type, then
//  1 (std)- mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z (end)
//  2 (test)- timePerTack (loggingInterval), z (end)

void parseRxBuffer(String rxBufferAsString) {
        rxMessageInvalid = false;
        if (rxBufferAsString.indexOf("z") >= 0) { // we should parse it
                switch ((rxBufferAsString.substring(0, rxBufferAsString.indexOf(','))).toInt()) { // get first value
                case 1: { // standard message = 1,120,20,60,20,30,z
                        sailMode = 'r'; // real sail mode
                        // find the commas, probably should move to char *?
                        byte firstCommaIndex = rxBufferAsString.indexOf(',');
                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);
                        byte thirdCommaIndex = rxBufferAsString.indexOf(',', secondCommaIndex + 1);
                        byte fourthCommaIndex = rxBufferAsString.indexOf(',', thirdCommaIndex + 1);
                        byte fifthCommaIndex = rxBufferAsString.indexOf(',', fourthCommaIndex + 1);
                        byte sixthCommaIndex = rxBufferAsString.indexOf(',', fifthCommaIndex + 1);
                        String firstValue = rxBufferAsString.substring(0, firstCommaIndex);
                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);
                        String thirdValue = rxBufferAsString.substring(secondCommaIndex + 1, thirdCommaIndex);
                        String fourthValue = rxBufferAsString.substring(thirdCommaIndex + 1, fourthCommaIndex);
                        String fifthValue = rxBufferAsString.substring(fourthCommaIndex + 1, fifthCommaIndex);
                        String sixthValue = rxBufferAsString.substring(fifthCommaIndex + 1, sixthCommaIndex);
                        // maybe restrict these values to 6 hr max and positive
                        orderedSailPositionA = secondValue.toInt(); // tolerates spaces
                        orderedTackTimeA = thirdValue.toInt();
                        orderedSailPositionB = fourthValue.toInt();
                        orderedTackTimeB = fifthValue.toInt();
                        unsigned long tempLoggingInterval = sixthValue.toInt();
                        if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) { // 86,400 secs = 1 day
                                loggingInterval = tempLoggingInterval;
                        }
                        break;
                }
                case 2: { // sail test mode 2,10,z is 10 min test
                        sailMode = 't'; // test sail mode
                        orderedSailPositionA = 180;
                        orderedTackTimeA = 1;
                        orderedSailPositionA = 180;
                        orderedTackTimeB = 1;
                        byte firstCommaIndex = rxBufferAsString.indexOf(',');
                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);
                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);
                        unsigned long tempLoggingInterval = secondValue.toInt();
                        if (tempLoggingInterval > 0 && tempLoggingInterval <= 86400) {
                                loggingInterval = tempLoggingInterval;
                        }
                        break;
                }
                case 3: { // not to be used in real sailing
                        sailMode = 's';
                        byte firstCommaIndex = rxBufferAsString.indexOf(',');
                        byte secondCommaIndex = rxBufferAsString.indexOf(',', firstCommaIndex + 1);
                        String secondValue = rxBufferAsString.substring(firstCommaIndex + 1, secondCommaIndex);
                        orderedSailPosition = secondValue.toInt();
                        loggingInterval = 1; // will wait 1 second before asking again
                        break;
                }
                default: {
                        Serial.println(F("Not a valid rx message type."));
                }
                }
        }
        else {
                rxMessageInvalid = true;
                Serial.println(F("No valid message (no z in message)"));
        }

        Serial.print(F("Logging interval = "));
        Serial.println(loggingInterval);
        Serial.print(F("Sail order A = "));
        Serial.print(orderedSailPositionA);
        Serial.print(F(" for "));
        Serial.print(orderedTackTimeA);
        Serial.println(F(" minutes"));
        Serial.print(F("Sail order B = "));
        Serial.print(orderedSailPositionB);
        Serial.print(F(" for "));
        Serial.print(orderedTackTimeB);
        Serial.println(F(" minutes"));
}


void blinkMessage(int condition) {
        switch (condition) {
        case 1: // continuous quick flash, stops program here
                while (true) {
                        digitalWrite(LED_PIN, (millis() / 100) % 2 == 1 ? HIGH : LOW); // rapid blink
                }
                break;
        case 2: // single quick flash
                digitalWrite(LED_PIN, HIGH);
                delay(DELAY_FOR_SERIAL);
                digitalWrite(LED_PIN, LOW);
                break;
        case 3: // continuous on 2, off half
                while (true) {
                        digitalWrite(LED_PIN, HIGH);
                        delay(2000);
                        digitalWrite(LED_PIN, LOW);
                        delay(500);
                }
                break;
        }
}

void getSerialMonitorOrders() {
        sail.GetPosition();
        Serial.println(F("Ready for orders:"));
        while (!Serial.available()) ; // wait for the serial data

        // get the order string, turn it into char[] b/c isbd returns that, turn back into string and split it at the commas
        String sentence = Serial.readString();
        size_t rxBufferSize = sentence.length() + 1;
        char rxBuffer[rxBufferSize];
        sentence.toCharArray(rxBuffer, rxBufferSize);
        String newString = (rxBuffer); // necessary?
        parseRxBuffer(newString); // could we do String(rxBuffer) or toString?
}

void getFakeOrders() {
        //  1 (std)- mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z (end)
        if (sailMode == 't') {
                String fakeOrderString = "3,";
                fakeOrderString += loggingInterval;
                fakeOrderString += ",z";
        }
        else {
                String fakeOrderString = "1,0,1,360,1,";
                fakeOrderString += loggingInterval;
                fakeOrderString += ",z";
                parseRxBuffer(fakeOrderString);
        }
}

byte diagnosticMessage() {
        // 1- too many trim pulses and sail is moving
        // 2- sail not moving after trim pulses
        // 4- fram problem
        // 8- rxMessage not valid
        // add these up to determine which combo is being reported
        return (trimRoutineExceededMax*pow(2,0) + sailNotMoving*pow(2,1) + rxMessageInvalid*pow(2,3));
}
