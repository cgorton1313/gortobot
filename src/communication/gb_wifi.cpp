#include <Arduino.h>
#include <communication/gb_wifi.h>

GbWifi::GbWifi(byte pin, HardwareSerial &port, unsigned long baud) {
    _wifi_enable_pin = pin;
    // try defining _gps_port first, then .begin it
    port.begin(baud);
    _wifi_port = &port;
}

bool GbWifi::UseWifi(String sentence) {
        bool send_receive_success = false;
        String _log_sentence = sentence;
        WifiOn();
        if (WifiReady()) {
                Serial.println(F("Wifi ready."));
                if (!WifiSend(_log_sentence)) {
                        Serial.println(F("WifiSend failed."));
                } else {
                        Serial.print(F("wifiSend succeeded. Message sent: "));
                        Serial.println(_log_sentence);
                        send_receive_success = true;
                }
                if (WifiReceive()); // need to refactor success criteria here
        } else {
                Serial.println(F("Wifi NOT ready."));
        }
        WifiOff();
        return send_receive_success;
}

void GbWifi::WifiOn() {
        digitalWrite(_wifi_enable_pin, HIGH);
        Serial.println(F("Wifi on."));
}

void GbWifi::WifiOff() {
        digitalWrite(_wifi_enable_pin, LOW);
        Serial.println(F("Wifi off."));
}

bool GbWifi::WifiReady() {
        unsigned long wifi_connect_start_time = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifi_connect_start_time) >= 10000) {
                        return false;
                }
                while (!_wifi_port->available()) ; // wait for the serial data
                String sentence = _wifi_port->readString();
                if (sentence.indexOf("zxzxzxz") >= 0) {
                        return true;
                }
        }
}

bool GbWifi::WifiSend(String tx_string) {
        _wifi_port->println(tx_string);
        unsigned long wifi_timer = millis(); // capture the time now
        while (true) {
                if ((unsigned long)(millis() - wifi_timer) >= 10000) { // 10 seconds has passed
                        return false;
                }
                while (!_wifi_port->available()) ; // wait for the serial data
                String sentence = _wifi_port->readString();
                if (sentence.indexOf("zzzxxxzzz") >= 0) {
                        return true;
                }
        }
}

bool GbWifi::WifiReceive() {
    return true;
}
