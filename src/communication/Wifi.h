#include <Arduino.h>

class Wifi {

private:
    byte _wifi_enable_pin;
    HardwareSerial* _wifi_port;
    void WifiOn();
    void WifiOff();
    bool WifiReady();
    bool WifiSend(String);
    bool WifiReceive();

public:
    Wifi(byte pin, HardwareSerial &port, unsigned long baud);
    void UseWifi(String sentence);
};
