/*
The wifi class is a wrapper for the wifi object and its power methods (on, off,
charge)
*/

#include <Arduino.h>
#include "../../src/configs/config.h"

#ifndef GB_WIFI_H
#define GB_WIFI_H

class GbWifi {

private:
  const uint32_t _WIFI_TIMEOUT = 20000; // in milliseconds
  const uint8_t _wifi_enable_pin;
  HardwareSerial *_wifi_port;
  void WifiOn();
  void WifiOff();
  bool WifiReady();
  bool WifiSend(String);
  bool WifiReceive();

public:
  GbWifi(uint8_t, HardwareSerial &port, uint32_t baud);
  bool UseWifi(String sentence);
};

#endif
