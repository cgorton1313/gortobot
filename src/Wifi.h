#include <Arduino.h>

class Wifi {

private:
    String _logSentence;
    byte _wifiEnablePin;
    HardwareSerial* _wifiPort;

public:
    Wifi(byte pin, HardwareSerial &port, String sentence);
    void useWifi();
    void wifiOn();
    void wifiOff();
    bool wifiReady();
    bool wifiSend(String);
    //bool wifiReceive();
};
