#include <Arduino.h>

class Battery {

private:
    byte _pin;
    float _voltage;
    float _min_voltage;
    float _retry_voltage;
    int _wait_time;
    bool _checking_voltage;

public:
    Battery(byte pin, float min_voltage, float retry_voltage, int wait_time, bool checking_voltage);
    float GetVoltage();
    void Okay();
};
