/*
A sailing orders object is simply a struct of the various parameters used to
tell the Gortobot what to do during each watch cycle.
*/

#include <stdint.h>

#ifndef GB_SAILING_ORDERS_H
#define GB_SAILING_ORDERS_H

struct GbSailingOrders {
  uint32_t loggingInterval;      // in seconds
  uint16_t orderedSailPositionA; // degrees b/w 0-359
  uint16_t orderedTackTimeA;     // in seconds
  uint16_t orderedSailPositionB; // degrees b/w 0-359
  uint16_t orderedTackTimeB;     // in seconds
};

#endif
