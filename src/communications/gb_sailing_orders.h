/*
A sailing orders object is simply a struct of the various parameters used to
tell the Gortobot what to do during each watch cycle.
*/

#include <stdint.h>

#ifndef GB_SAILING_ORDERS_H
#define GB_SAILING_ORDERS_H

struct GbSailingOrders {
  uint32_t loggingInterval;
  uint16_t orderedSailPositionA;
  uint16_t orderedTackTimeA;
  uint16_t orderedSailPositionB;
  uint16_t orderedTackTimeB;
};

#endif
