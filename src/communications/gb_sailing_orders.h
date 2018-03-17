#include <Arduino.h>

#ifndef gb_sailing_orders_h
#define gb_sailing_orders_h

struct GbSailingOrders {
    // TODO enum these
    // Sail modes: r=real, t=testing, s=direct set, p=pulse, f=fake
    char sailMode;
    unsigned long loggingInterval;
    int orderedSailPositionA;
    int orderedTackTimeA;
    int orderedSailPositionB;
    int orderedTackTimeB;
};

#endif
