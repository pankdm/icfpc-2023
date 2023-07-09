#pragma once

#include "base/constants.h"
#include "base/problem.h"
#include "base/solution.h"

class AdjusterMaxVolume {
 public:
  static bool Check(const Problem&, Solution& s) {
    if (s.volume.empty()) {
      s.volume.resize(s.positions.size(), max_volume);
      return true;
    }
    return false;
  }
};
