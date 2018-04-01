#include <Vcc.h>

#ifndef GB_BATTERY_H
#define GB_BATTERY_H

class GbBattery {

private:
    uint8_t _battery_number;
    uint8_t _pin;
    float _voltage;
    float _min_voltage;
    float _retry_voltage;
    uint16_t _wait_time;
    bool _checking_voltage;

public:
    GbBattery(uint8_t battery_number, uint8_t pin, float min_voltage, float retry_voltage, uint16_t wait_time, bool checking_voltage);
    float GetVoltage();
    char Status();
};

#endif
