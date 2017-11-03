#include <Arduino.h>

class GbWifi {

private:
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
