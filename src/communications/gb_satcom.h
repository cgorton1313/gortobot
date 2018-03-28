#include <Arduino.h>
#include <IridiumSBD.h>

#ifndef GB_SATCOM_H
#define GB_SATCOM_H

class GbSatcom {

private:
const uint32_t _satcom_baud;
HardwareSerial _satcom_port;
IridiumSBD _isbd;
String _inboundMessage;
void SatOn();
void SatOff();
void ChargeSuperCapacitor(uint16_t chargeTime);

public:
GbSatcom(uint8_t sleepPin, HardwareSerial &port, uint32_t baud);
void SetUpSat(uint16_t chargeTime, uint16_t timeOut);
//bool UseMockSatcom(String txString);
bool UseSatcom(String sentence);
String GetInboundMessage();
};

#endif
