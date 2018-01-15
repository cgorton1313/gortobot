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

void GbBattery::Okay() {
        float voltage_now;
        bool voltage_critical = false;

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

        if (voltage_now < _min_voltage) {
                voltage_critical = true;
        }
        while (voltage_critical) {
                Sleep sleep;
                sleep.pwrDownMode(); // best power saving mode for sleeping
                Serial.print(F("Battery"));
                Serial.print(_battery_number);
                Serial.print(F(" voltage critical! Voltage = "));
                Serial.print(voltage_now);
                Serial.print(F(". Waiting "));
                Serial.print(_wait_time);
                Serial.println(F(" seconds."));
                for (int i = 0; i < _wait_time; i++) {
                        sleep.sleepDelay(1000);
                }
                voltage_now = this->GetVoltage();
                if (voltage_now > _retry_voltage) {
                        voltage_critical = false;
                }
        }
        Serial.print(F("Battery"));
        Serial.print(_battery_number);
        Serial.print(F(" voltage okay. Voltage = "));
        Serial.println(voltage_now);
}
