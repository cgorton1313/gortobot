#include <Arduino.h>

class GbSatcom {

private:
    unsigned long _satcom_baud;
    HardwareSerial* _satcom_port;
    String _inboundMessage;
    void SatOn();
    void SatOff();
    void ChargeSuperCapacitor(int chargeTime);

public:
    GbSatcom(byte sleepPin, unsigned long baud);
    void SetUpSat(int chargeTime, int timeOut);
    //bool UseMockSatcom(String txString);
    bool UseSatcom(String sentence);
    String GetInboundMessage();
};
