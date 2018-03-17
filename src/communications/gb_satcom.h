#include <Arduino.h>

class GbSatcom {

private:
    uint32_t _satcom_baud;
    HardwareSerial* _satcom_port;
    String _inboundMessage;
    void SatOn();
    void SatOff();
    void ChargeSuperCapacitor(uint16_t chargeTime);

public:
    GbSatcom(uint8_t sleepPin, uint32_t baud);
    void SetUpSat(uint16_t chargeTime, uint16_t timeOut);
    //bool UseMockSatcom(String txString);
    bool UseSatcom(String sentence);
    String GetInboundMessage();
};
