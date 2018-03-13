#include <Arduino.h>

#ifndef gb_sailing_orders_h
#define gb_sailing_orders_h

struct GbSailingOrders {
    char sailMode;
    unsigned long loggingInterval;
    int orderedSailPositionA;
    int orderedTackTimeA;
    int orderedSailPositionB;
    int orderedTackTimeB;
};

#endif
