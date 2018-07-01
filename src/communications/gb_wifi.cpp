#include "gb_wifi.h"

GbWifi::GbWifi(uint8_t pin, HardwareSerial &port, uint32_t baud)
    : _wifi_enable_pin(pin), _wifi_port(&port) {
  pinMode(_wifi_enable_pin, OUTPUT);
  digitalWrite(_wifi_enable_pin, LOW);
  _wifi_port->begin(baud);
}

bool GbWifi::UseWifi(String sentence) {
  bool send_receive_success = false;
  String _log_sentence = sentence;
  WifiOn();
  if (WifiReady()) {
    if (!WifiSend(_log_sentence)) {
      DEBUG_PRINTLN(F("WifiSend failed."));
    } else {
      DEBUG_PRINT(F("wifiSend succeeded. Message sent: "));
      DEBUG_PRINTLN(_log_sentence);
      send_receive_success = true;
    }
    if (WifiReceive())
      ; // need to refactor success criteria here
  }
  WifiOff();
  delay(1000); // let wifi reset
  return send_receive_success;
}

bool GbWifi::WifiReady() {
  DEBUG_PRINTLN(F("Waiting for wifi ready..."));
  uint32_t wifi_connect_start_time = millis(); // capture the time now
  while (true) {
    while (!_wifi_port->available()) { // wait for the serial data
      digitalWrite(LED_BUILTIN,
                   (millis() % 250 > 125)); // flash every 250ms while not ready
      if ((uint32_t)(millis() - wifi_connect_start_time) >= _WIFI_TIMEOUT) {
        DEBUG_PRINTLN(F("Timed out waiting for wifi_port.available"));
        return false;
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
    String sentence = _wifi_port->readString();
    if (sentence.indexOf("zxzxzxz") >= 0) {
      DEBUG_PRINTLN(F("Wifi ready."));
      return true;
    }
    if ((unsigned long)(millis() - wifi_connect_start_time) >= _WIFI_TIMEOUT) {
      DEBUG_PRINTLN(F("Timed out waiting for zxzxzxz."));
      return false;
    }
  }
}

bool GbWifi::WifiSend(String tx_string) {
  DEBUG_PRINTLN(F("Waiting for bytes on wifi_port..."));
  _wifi_port->println(tx_string);
  unsigned long wifi_timer = millis(); // capture the time now

  while (!_wifi_port->available()) { // wait for the serial data
    digitalWrite(LED_BUILTIN, (millis() % 100 > 50));
    if ((unsigned long)(millis() - wifi_timer) >=
        _WIFI_TIMEOUT) { // 10 seconds has passed
      DEBUG_PRINTLN(F("WifiSend timed out!"));
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

bool GbWifi::WifiReceive() { return true; }

void GbWifi::WifiOn() {
  digitalWrite(_wifi_enable_pin, HIGH);
  DEBUG_PRINTLN(F("Wifi on."));
}

void GbWifi::WifiOff() {
  digitalWrite(_wifi_enable_pin, LOW);
  DEBUG_PRINTLN(F("Wifi off."));
}
