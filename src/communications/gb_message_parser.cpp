#include "gb_message_parser.h"

// Private constructor for static class
GbMessageParser::GbMessageParser() {}

GbSailingOrders GbMessageParser::ParseMessage(String inboundMessage) {
  GbSailingOrders newSailingOrders;
  newSailingOrders.loggingInterval = 60;
  newSailingOrders.orderedSailPositionA = 270;
  newSailingOrders.orderedTackTimeA = 90;
  newSailingOrders.orderedSailPositionB = 270;
  newSailingOrders.orderedTackTimeB = 90;
  return newSailingOrders;
}
