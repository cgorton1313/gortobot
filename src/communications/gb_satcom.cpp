#include "gb_satcom.h"
#include <IridiumSBD.h>

GbSatcom::GbSatcom(byte pin, HardwareSerial &port, unsigned int baud) {
        _satcom_sleep_pin = pin;
        port.begin(baud);
        _satcom_port = &port;
        IridiumSBD isbd(port, pin);
}

bool GbSatcom::UseSatcom(String sentence) {
    return false;
}
