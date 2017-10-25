#include <Arduino.h>
#include <communication/Wifi.h>

Wifi::Wifi(byte pin, HardwareSerial &port, unsigned long baud) {
    _wifiEnablePin = pin;
    port.begin(baud);
    _wifiPort = &port;
}

void Wifi::UseWifi(String sentence) {
        String _logSentence = sentence;
        WifiOn();
        if (WifiReady()) {
                Serial.println(F("Wifi ready."));
                if (!WifiSend(_logSentence)) {
                        Serial.println(F("WifiSend failed."));
                } else
                {
                        Serial.print(F("wifiSend succeeded. Message sent: "));
                        Serial.println(_logSentence);
                }
                if (WifiReceive());
        } else {
                Serial.println(F("Wifi NOT ready."));
        }
        WifiOff();
}

void Wifi::WifiOn() {
        digitalWrite(_wifiEnablePin, HIGH);
        Serial.println(F("Wifi on."));
}

void Wifi::WifiOff() {
        digitalWrite(_wifiEnablePin, LOW);
        Serial.println(F("Wifi off."));
}

bool Wifi::WifiReady() {
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 1 minute has passed
                        return false;
                }
                while (!_wifiPort->available()) ; // wait for the serial data
                String sentence = _wifiPort->readString();
                if (sentence.indexOf("zxzxzxz") >= 0) {
                        return true;
                }
        }
}

bool Wifi::WifiSend(String txString) {
        _wifiPort->println(txString);
        unsigned long wifiTimer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifiTimer) >= 10000) { // 10 seconds has passed
                        return false;
                }
                while (!_wifiPort->available()) ; // wait for the serial data
                String sentence = _wifiPort->readString();
                if (sentence.indexOf("zzzxxxzzz") >= 0) {
                        return true;
                }
        }
}

bool Wifi::WifiReceive() {
    return true;
}
