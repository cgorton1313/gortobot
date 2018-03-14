#include <Arduino.h>
#include <gb_sail.h>
#include "communications/gb_sailing_orders.h"

#ifndef gb_watch_stander_h
#define gb_watch_stander_h

class GbWatchStander {

private:

public:
GbWatchStander();
void StandWatch(GbSail sail, GbSailingOrders sailingOrders);
void PulseSail(GbSail sail);
void FakeSail(unsigned long watchDuration);
};

#endif
