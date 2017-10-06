void useWifi() {
        wifiOn();
        if (wifiReady()) {
                Serial.println(F("Wifi ready."));
                if (!wifiSend(logSentence)) {
                        Serial.println(F("wifiSend failed."));
                } else {
                        Serial.print(F("wifi sent "));
                        Serial.println(logSentence);
                }
                wifiReceive();
        } else {
                Serial.println(F("Wifi NOT ready."));
        }
        wifiOff();
}

void wifiOn() {
        digitalWrite(wifiEnablePin, HIGH);
        Serial.println(F("Wifi on."));
}

void wifiOff() {
        digitalWrite(wifiEnablePin, LOW);
        Serial.println(F("Wifi off."));
}

boolean wifiReady() {
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 1 minute has passed
                        return false;
                }
                while (!wifiPort.available()) ; // wait for the serial data
                String sentence = wifiPort.readString();
                //Serial.println(sentence);
                if (sentence.indexOf("zxzxzxz") >= 0) {
                        return true;
                }
        }
}

boolean wifiSend(String txString) {
        wifiPort.println(txString);
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 10 seconds has passed
                        return false;
                }
                while (!wifiPort.available()) ; // wait for the serial data
                String sentence = wifiPort.readString();
                if (sentence.indexOf("zzzxxxzzz") >= 0) {
                        return true;
                }
        }
}

void wifiReceive() {}
