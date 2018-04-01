/*
A sailing orders object is simply a struct of the various parameters used to
control the Gortobot. 
*/

#include <stdint.h>

#ifndef GB_SAILING_ORDERS_H
#define GB_SAILING_ORDERS_H

struct GbSailingOrders {
    // TODO enum these
    // Sail modes: r=real, t=testing, s=direct set, p=pulse, f=fake
    char sailMode;
    bool validOrders;
    uint32_t loggingInterval;
    uint16_t orderedSailPositionA;
    uint16_t orderedTackTimeA;
    uint16_t orderedSailPositionB;
    uint16_t orderedTackTimeB;
};

#endif
