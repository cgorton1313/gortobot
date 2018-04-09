/*
The watch stander executes the sailing orders.
*/

#include "communications/gb_sailing_orders.h"
#include "communications/gb_blinker.h"
#include "sailing/gb_sail.h"

#ifndef GB_WATCH_STANDER_H
#define GB_WATCH_STANDER_H

class GbWatchStander {

private:
  uint16_t _current_orderedSailPosition;
  bool _tackIsA;
  uint16_t _currentTackTime;
  void RealSail(GbSail sail, GbSailingOrders sailingOrders);
  void PulseSail(GbSail sail);
  void FakeSail(unsigned long watchDuration);
  void TestSail(GbSail sail, GbSailingOrders sailingOrders);
  GbBlinker _blinker;

public:
  GbWatchStander(uint8_t ledPin);
  void StandWatch(GbSail sail, GbSailingOrders sailingOrders);
};

#endif
