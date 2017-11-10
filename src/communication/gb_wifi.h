#include <Arduino.h>

#ifndef gb_wifi_h
#define gb_wifi_h

class GbWifi {

private:
    const unsigned long _WIFI_TIMEOUT = 20000; // in milliseconds
    byte _wifi_enable_pin;
    HardwareSerial* _wifi_port;
    void WifiOn();
    void WifiOff();
    bool WifiReady();
    bool WifiSend(String);
    bool WifiReceive();

public:
    GbWifi(byte pin, HardwareSerial &port, unsigned long baud);
    bool UseWifi(String sentence);
};

#endif
