#include <Arduino.h>

class Wifi {

private:
    byte _wifiEnablePin;
    HardwareSerial* _wifiPort;
    void wifiOn();
    void wifiOff();
    bool wifiReady();
    bool wifiSend(String);
    bool wifiReceive();

public:
    Wifi(byte pin, HardwareSerial &port);
    void useWifi(String sentence);
};
