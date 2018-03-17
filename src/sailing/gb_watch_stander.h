#include "communications/gb_sailing_orders.h"
#include "communications/gb_blinker.h"
#include <Arduino.h>
#include <gb_sail.h>

#ifndef gb_watch_stander_h
#define gb_watch_stander_h

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
