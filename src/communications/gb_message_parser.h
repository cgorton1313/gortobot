#include <Arduino.h>
#include "gb_sailing_orders.h"

#ifndef gb_message_parser_h
#define gb_message_parser_h

class GbMessageParser {

private:
GbMessageParser();

public:
static GbSailingOrders ParseMessage(String inboundMessage);
};

#endif
