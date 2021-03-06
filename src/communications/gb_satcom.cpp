#include "gb_satcom.h"

#define _diagnostics false // get serial diagnostics for isbd

GbSatcom::GbSatcom(uint8_t sleepPin, HardwareSerial &port, uint32_t baud)
    : _satcom_baud(baud), _satcom_port(port), _isbd(port, sleepPin) {
  pinMode(sleepPin, OUTPUT);
  digitalWrite(sleepPin, LOW);
}

void GbSatcom::SetUpSat(uint16_t chargeTime, uint16_t timeOut) {

  ChargeSuperCapacitor(chargeTime);

  // For battery set ups, where plenty of power can recharge the super capacitor
  _isbd.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);

  _isbd.adjustSendReceiveTimeout(timeOut);

  _isbd.useMSSTMWorkaround(false); // since we have a newer RockBLOCK
}

bool GbSatcom::UseSatcom(String txString) {
  // Clear out the last inbound message
  _inboundMessage = "";

  SatOn();
  do {
    // Set up TX (transmit) buffer
    char txBuffer[50];
    txString.toCharArray(txBuffer, txString.length() + 1);

    // Set up RX (receive) buffer
    char rxBuffer[27]; // max size of rx buffer
    // expects type(max 1 char),tackAsailPosition(max 3 chars),
    // tackAtime(max 4 chars),tackBsailPosition(max 3 chars),
    // tackBtime(max 4 chars),loggingInterval(maz 4 chars),z
    // = max of 26
    // e.g., "1,359,1439,359,1439,1439,z" is biggest message
    // extra size for 0 terminated? I think that's why
    size_t rxBufferSize = sizeof(rxBuffer);

    // Try to send the message (always) and receive message (if present)
    uint16_t error =
        _isbd.sendReceiveSBDText(txBuffer, (uint8_t *)rxBuffer, rxBufferSize);
    if (error != 0) {
      DEBUG_PRINT(F("sendReceiveSBDText failed. Error: "));
      DEBUG_PRINTLN(error);
      SatOff();
      return false;
    }

    // Handle inbound message. Only the last inbound message will survive this
    // loop.
    DEBUG_PRINT(F("rxBufferSize = "));
    DEBUG_PRINTLN(rxBufferSize);
    if (rxBufferSize == 0) {
      DEBUG_PRINTLN(F("No message received"));
      _inboundMessage = '0';
    } else {
      DEBUG_PRINT(F("Message received: "));
      DEBUG_PRINTLN(rxBuffer);
      _inboundMessage = rxBuffer;
      DEBUG_PRINT(F("Messages left: "));
      DEBUG_PRINTLN(_isbd.getWaitingMessageCount());
    }
  } while (_isbd.getWaitingMessageCount() > 0);
  SatOff();
  return true;
}

String GbSatcom::GetInboundMessage() { return _inboundMessage; }

void GbSatcom::SatOn() {
  DEBUG_PRINTLN(F("Sat on."));
  _satcom_port.begin(_satcom_baud);
  _isbd.begin();
}

void GbSatcom::SatOff() {
  _isbd.sleep();
  _satcom_port.end();
  digitalWrite(LED_BUILTIN, LOW); // to insure blinker didn't stay on
  DEBUG_PRINTLN(F("Sat off."));
}

void GbSatcom::ChargeSuperCapacitor(uint16_t chargeTime) {
  DEBUG_PRINT(F("Charging super-capacitor. Waiting "));
  DEBUG_PRINT(chargeTime);
  DEBUG_PRINTLN(F(" seconds..."));
  uint16_t delayTime = chargeTime * 1000;
  delay(delayTime);
}

bool ISBDCallback() {
  // This runs while ISBD is trying send/receive
  // Two quick flashes every second
  bool ledOn;
  if ((millis() / 50 % 20 == 0) || (millis() / 50 % 20 == 5)) {
    ledOn = true;
  } else {
    ledOn = false;
  }
  digitalWrite(LED_BUILTIN, ledOn);
  return true;
}

void ISBDConsoleCallback(IridiumSBD *device, char c) {
#if _diagnostics
  Serial.write(c);
#endif
}

void ISBDDiagsCallback(IridiumSBD *device, char c) {
#if _diagnostics
  Serial.write(c);
#endif
}
