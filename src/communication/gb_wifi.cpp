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
                if (!WifiSend(_log_sentence)) {
                        Serial.println(F("WifiSend failed."));
                } else {
                        Serial.print(F("wifiSend succeeded. Message sent: "));
                        Serial.println(_log_sentence);
                        send_receive_success = true;
                }
                if (WifiReceive()) ; // need to refactor success criteria here
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
        Serial.println(F("Waiting for wifi ready..."));
        unsigned long wifi_connect_start_time = millis(); // capture the time now
        while (true) {
                while (!_wifi_port->available()) { // wait for the serial data
                        digitalWrite(LED_BUILTIN, (millis()%250 > 125) ); // flash every 250ms while not ready
                        if ((unsigned long)(millis() - wifi_connect_start_time) >= _WIFI_TIMEOUT) {
                                Serial.println(F("Timed out waiting for wifi_port.available"));
                                return false;
                        }
                }
                digitalWrite(LED_BUILTIN, LOW);
                String sentence = _wifi_port->readString();
                if (sentence.indexOf("zxzxzxz") >= 0) {
                        Serial.println(F("Wifi ready."));
                        return true;
                }
                if ((unsigned long)(millis() - wifi_connect_start_time) >= _WIFI_TIMEOUT) {
                        Serial.println(F("Timed out waiting for zxzxzxz."));
                        return false;
                }
        }
}

bool GbWifi::WifiSend(String tx_string) {
        Serial.println(F("Waiting for bytes on wifi_port..."));
        _wifi_port->println(tx_string);
        unsigned long wifi_timer = millis(); // capture the time now

        while (!_wifi_port->available()) {         // wait for the serial data
                digitalWrite(LED_BUILTIN, (millis()%100 > 50) );
                if ((unsigned long)(millis() - wifi_timer) >= _WIFI_TIMEOUT) {     // 10 seconds has passed
                        Serial.println(F("WifiSend timed out!"));
                        return false;
                }
        }
        digitalWrite(LED_BUILTIN, LOW);
        String sentence = _wifi_port->readString();
        if (sentence.indexOf("zzzxxxzzz") >= 0) {
                return true;
        } else {
                return false;
        }

}

bool GbWifi::WifiReceive() {
        return true;
}
