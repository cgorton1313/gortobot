#include <Arduino.h>
#include "gb_sailing_orders.h"

// MT message starts with message type, then
//  1 (std)-
//  mastPositionA(min-max),timeA(minutes),mastPositionB(min-max),timeB(minutes),loggingInterval(min),z
//  (end)
//  2 (test)- timePerTack (loggingInterval), z (end)

#ifndef GB_MESSAGE_HANDLER_H
#define GB_MESSAGE_HANDLER_H

class GbMessageHandler {

public:
GbSailingOrders ParseMessage(String inboundMessage);
String GetSerialMessage();
String GetFakeMessage(GbSailingOrders sailingOrders);
uint8_t GetDiagnosticMessage();

};

#endif
