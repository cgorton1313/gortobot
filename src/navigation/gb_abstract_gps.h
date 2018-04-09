#include "gb_fix.h"

#ifndef GB_ABSTRACT_GPS_H
#define GB_ABSTRACT_GPS_H

class GbAbstractGps {

public:
  GbAbstractGps(){};
  virtual GbFix GetFix() = 0;
};

#endif
