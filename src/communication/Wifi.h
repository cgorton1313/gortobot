#include <Arduino.h>

class Wifi {

private:
    byte _wifiEnablePin;
    HardwareSerial* _wifiPort;
    void WifiOn();
    void WifiOff();
    bool WifiReady();
    bool WifiSend(String);
    bool WifiReceive();

public:
    Wifi(byte pin, HardwareSerial &port, unsigned long baud);
    void UseWifi(String sentence);
};
