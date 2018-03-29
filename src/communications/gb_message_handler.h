#include <Arduino.h>
#include "navigation/gb_fix.h"
#include "communications/gb_sailing_orders.h"

// MT message starts with message type, then
//  1 (std)-
//  mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z
//  (end)
//  2 (test)- timePerTack (loggingInterval), z (end)

#ifndef GB_MESSAGE_HANDLER_H
#define GB_MESSAGE_HANDLER_H

class GbMessageHandler {

private:
  uint8_t _message_version;
  String FormatDateNumber(uint8_t number);
  String ConvertToBase62(uint32_t input);

public:
  GbMessageHandler(uint8_t message_version);
  String BuildOutboundMessage(uint16_t run_num, uint16_t loop_count, GbFix &a_fix,
      float battery_voltage, float battery2_voltage, int sail_position, uint8_t diagnostic_message);
  GbSailingOrders ParseMessage(String inboundMessage);
  String GetSerialMessage();
  String GetFakeMessage(GbSailingOrders sailingOrders);
  uint8_t GetDiagnosticMessage();
};

#endif
