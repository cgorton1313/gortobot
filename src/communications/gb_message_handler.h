/*
The message handler takes various data points and constructs the string
that gets transmited back to shore.
It also converts a received message into a sailing order object to be
used to tell Gorotbot what to do.
*/

#include "gb_sailing_orders.h"
#include "../navigation/gb_fix.h"
#include "../sailing/gb_sail.h"
#include <Arduino.h>

/*
Mobile terminated (MT) message string:
  messageType,mastPositionA,timeA(minutes),
  mastPositionB,timeB(minutes),loggingInterval(minutes),z
  Example: 1,90,15,270,45,120,z
*/

#ifndef GB_MESSAGE_HANDLER_H
#define GB_MESSAGE_HANDLER_H

class GbMessageHandler {

private:
  String FormatDateNumber(uint8_t number);
  String ConvertToBase62(int32_t input);
  bool ValidSailingOrders(GbSailingOrders ordersToCheck);
  String LongFormBase(uint16_t run_num, uint32_t loop_count, GbFix &a_fix,
                      float battery_voltage, float battery2_voltage,
                      int sail_position, uint8_t diagnostic_message);
  String ShortFormBase(uint16_t run_num, uint32_t loop_count, GbFix &a_fix,
                       float battery_voltage, float battery2_voltage,
                       int sail_position, uint8_t diagnostic_message);

public:
  String BuildOutboundMessage(uint8_t message_version, uint16_t run_num,
                              uint32_t loop_count, GbFix &a_fix,
                              float battery_voltage, float battery2_voltage,
                              int sail_position, uint8_t diagnostic_message);
  String BuildOutboundMessage(uint8_t message_version, uint16_t run_num,
                              uint32_t loop_count, GbFix &a_fix,
                              float battery_voltage, float battery2_voltage,
                              int sail_position, uint8_t diagnostic_message,
                              float temperature, float humidity);

  GbSailingOrders ParseMessage(String inboundMessage);
  uint8_t GetDiagnosticMessage(GbTrimResult trimResult, bool rxMessageInvalid);
  bool IsValidInboundMessage(String inboundMessage);
};

#endif
