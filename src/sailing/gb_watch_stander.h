#include "communications/gb_sailing_orders.h"
#include <Arduino.h>
#include <gb_sail.h>

#ifndef gb_watch_stander_h
#define gb_watch_stander_h

class GbWatchStander {

private:
  int _current_orderedSailPosition;
  bool _tackIsA;
  int _currentTackTime;
  void RealSail(GbSail sail, GbSailingOrders sailingOrders);
  void PulseSail(GbSail sail);
  void FakeSail(unsigned long watchDuration);
  void TestSail(GbSail sail, GbSailingOrders sailingOrders);

public:
  GbWatchStander();
  void StandWatch(GbSail sail, GbSailingOrders sailingOrders);
};

#endif
