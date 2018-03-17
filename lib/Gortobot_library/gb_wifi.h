#include <Arduino.h>

#ifndef gb_wifi_h
#define gb_wifi_h

class GbWifi {

private:
    const unsigned long _WIFI_TIMEOUT = 20000; // in milliseconds
    uint8_t _wifi_enable_pin;
    HardwareSerial* _wifi_port;
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
