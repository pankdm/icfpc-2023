#pragma once

#include "common/geometry/d2/point.h"

#include <vector>

class Attendee {
 public:
  D2Point position;
  std::vector<double> tastes;
};
