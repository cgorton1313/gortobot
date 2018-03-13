#include "gb_satcom.h"
#include "utilities/gb_utility.h"
#include <IridiumSBD.h>

IridiumSBD *_isbd; // TODO: we maybe don't  need a destructor
unsigned long _satcom_baud;
String _inboundMessage;

GbSatcom::GbSatcom(byte sleepPin, unsigned long baud) {
  _satcom_baud = baud;
  _isbd = new IridiumSBD(Serial3, sleepPin);
}

void GbSatcom::SetUpSat(int chargeTime, int timeOut) {

  ChargeSuperCapacitor(chargeTime);

  // TODO: new ISBD interface in 2.0?
  // isbd.attachConsole(Serial); // lets me see what the sat modem is doing

  // For battery set ups, where plenty of power can recharge the super capacitor
  _isbd->setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);

  // TODO: necessary?
  _isbd->adjustSendReceiveTimeout(timeOut);

  // isbd.setMinimumSignalQuality(3); // default is 2, trying this for stability

  // See erratum in ISBD docs. This work around si not needed for firmware since
  // 2013
  _isbd->useMSSTMWorkaround(
      false); // I think I need this here, which is a good thing
}

// bool GbSatcom::UseMockSatcom(String txString) {
//   char rxBuffer[27] = "1,359,1439,359,1439,1439,z";
//   _inboundMessage = rxBuffer;
//   return true;
// }

bool GbSatcom::UseSatcom(String txString) {
  // Clear out the last inbound message`
  _inboundMessage = "";

  SatOn();
  do {
    // Set up tx (transmit) buffer
    char txBuffer[50];
    txString.toCharArray(txBuffer, txString.length() + 1);

    // Set up rx (receive) buffer
    char rxBuffer[27]; // max size of rx buffer
    // expects type(max 1 char),tackAsailPosition(max 3 chars),
    // tackAtime(max 4 chars),tackBsailPosition(max 3 chars),
    // tackBtime(max 4 chars),loggingInterval(maz 4 chars),z
    // = max of 26
    // e.g., "1,359,1439,359,1439,1439,z" is biggest message
    // extra size for 0 terminated? I think that's why
    size_t rxBufferSize = sizeof(rxBuffer);

    // Try to send the message (always) and receive message (if present)
    int error =
        _isbd->sendReceiveSBDText(txBuffer, (uint8_t *)rxBuffer, rxBufferSize);
    if (error != 0) {
      Serial.print(F("sendReceiveSBDText failed. Error: "));
      Serial.println(error);
      SatOff();
      return false;
    }

    // Handle inbound message. Only the last inbound message will survive this
    // loop.
    Serial.print(F("rxBufferSize = "));
    Serial.println(rxBufferSize);
    if (rxBufferSize == 0) {
      Serial.println(F("No message received"));
    } else {
      Serial.print(F("Message received: "));
      Serial.println(rxBuffer);
      _inboundMessage = rxBuffer;
      Serial.print(F("Messages left: "));
      Serial.println(_isbd->getWaitingMessageCount());
    }
  } while (_isbd->getWaitingMessageCount() > 0);
  SatOff();
  return true;
}

String GbSatcom::GetInboundMessage() { return _inboundMessage; }

void GbSatcom::SatOn() {
  Serial.println(F("Sat on."));
  Serial3.begin(_satcom_baud);
  _isbd->begin();
}

void GbSatcom::SatOff() {
  _isbd->sleep();
  Serial3.end();
  // blinkMessage(2); // makes sure led doesn't get left ON by mistake
  Serial.println(F("Sat off."));
}

void GbSatcom::ChargeSuperCapacitor(int chargeTime) {
  Serial.print(F("Charging super-capacitor. Waiting "));
  Serial.print(chargeTime);
  Serial.println(F(" seconds..."));
  GbUtility::GortoNap(chargeTime); // allow capacitor to charge
}
