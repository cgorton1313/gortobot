#ifndef GB_TRIM_RESULT_H
#define GB_TRIM_RESULT_H

struct GbTrimResult {
  bool success;
  bool sailStuck;
  bool trimRoutineExceededMax;
  bool sailBatteryTooLow;
  bool invalidSailPositionOrder;
};

#endif
