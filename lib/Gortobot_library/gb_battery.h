#include <Arduino.h>
#include <Vcc.h>

class GbBattery {

private:
    byte _battery_number;
    byte _pin;
    float _voltage;
    float _min_voltage;
    float _retry_voltage;
    int _wait_time;
    bool _checking_voltage;

public:
    GbBattery(byte battery_number, byte pin, float min_voltage, float retry_voltage, int wait_time, bool checking_voltage);
    float GetVoltage();
    boolean Okay();
};
