#include <Arduino.h>

class GbSatcom {

private:
    // const unsigned long _WIFI_TIMEOUT = 20000; // in milliseconds
    byte _satcom_sleep_pin;
    HardwareSerial* _satcom_port;
    // IridiumSBD _isbd;
    // void WifiOn();
    // void WifiOff();
    // bool WifiReady();
    // bool WifiSend(String);
    // bool WifiReceive();

public:
    GbSatcom(byte pin, HardwareSerial &port, unsigned int baud);
    bool UseSatcom(String sentence);
};
