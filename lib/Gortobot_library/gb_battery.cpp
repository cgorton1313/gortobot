#include <Arduino.h>
#include "gb_battery.h"
#include <Sleep_n0m1.h>

GbBattery::GbBattery(byte battery_number, byte pin, float min_voltage, float retry_voltage, int wait_time, bool checking_voltage) {
        _battery_number = battery_number;
        _pin = pin;
        _min_voltage = min_voltage;
        _retry_voltage = retry_voltage;
        _wait_time = wait_time;
        _checking_voltage = checking_voltage;
}

float GbBattery::GetVoltage() {
        if (_checking_voltage) {
                delay(2000); // settle down
                int battery_voltage_int = 0;
                const int samples = 10; // number of samples to take
                for (int i = 0; i < samples; i++) {
                        delay(5);
                        battery_voltage_int = battery_voltage_int + analogRead(_pin);
                }
                Vcc vcc(1); // VccCorrection
                return (vcc.Read_Volts() * (((float)battery_voltage_int / samples) / 1023.0));
        } else {
                return 3.99;
        }
}

char GbBattery::Status() {
        float voltage_now;

        if (_checking_voltage) {
                voltage_now = this->GetVoltage();
                Serial.print(F("Battery"));
                Serial.print(_battery_number);
                Serial.print(F(" voltage = "));
        } else {
                voltage_now = 3.99;
                Serial.print(F("Fake voltage = "));
        }
        Serial.println(voltage_now);

        char status;
        if (voltage_now < _min_voltage) {
            status = 'r'; // red, bad
        } else if (voltage_now > _retry_voltage){
            status = 'g'; // green, good
        } else {
            status = 'y'; // yellow, wait a bit
        }
        return status;
}
