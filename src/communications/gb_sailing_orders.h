#include <Arduino.h>

#ifndef gb_sailing_orders_h
#define gb_sailing_orders_h

struct GbSailingOrders {
    // TODO enum these
    // Sail modes: r=real, t=testing, s=direct set, p=pulse, f=fake
    char sailMode;
    uint32_t loggingInterval;
    uint16_t orderedSailPositionA;
    uint16_t orderedTackTimeA;
    uint16_t orderedSailPositionB;
    uint16_t orderedTackTimeB;
};

#endif
