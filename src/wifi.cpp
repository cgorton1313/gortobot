#include <Arduino.h>

class Wifi {

private:
    String _logSentence;
    byte _wifiEnablePin;

public:
Wifi(byte pin, String sentence) {
    _logSentence = sentence;
    _wifiEnablePin = pin;
}

void useWifi() {
        wifiOn();
        if (wifiReady()) {
                Serial.println(F("Wifi ready."));
                if (!wifiSend(_logSentence)) {
                        Serial.println(F("wifiSend failed."));
                } else {
                        Serial.print(F("wifi sent "));
                        Serial.println(_logSentence);
                }
                wifiReceive();
        } else {
                Serial.println(F("Wifi NOT ready."));
        }
        wifiOff();
}

void wifiOn() {
        digitalWrite(_wifiEnablePin, HIGH);
        Serial.println(F("Wifi on."));
}

void wifiOff() {
        digitalWrite(_wifiEnablePin, LOW);
        Serial.println(F("Wifi off."));
}

bool wifiReady() {
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 1 minute has passed
                        return false;
                }
                while (!Serial2.available()) ; // wait for the serial data
                String sentence = Serial2.readString();
                //Serial.println(sentence);
                if (sentence.indexOf("zxzxzxz") >= 0) {
                        return true;
                }
        }
}

bool wifiSend(String txString) {
        Serial2.println(txString);
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 10 seconds has passed
                        return false;
                }
                while (!Serial2.available()) ; // wait for the serial data
                String sentence = Serial2.readString();
                if (sentence.indexOf("zzzxxxzzz") >= 0) {
                        return true;
                }
        }
}

bool wifiReceive() {
}

};
