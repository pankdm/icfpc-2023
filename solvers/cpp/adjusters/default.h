#pragma once

#include "adjusters/assignment.h"
#include "adjusters/max_volume.h"
#include "adjusters/swaps.h"
#include "base/problem.h"
#include "base/solution.h"

class AdjusterDefault {
 public:
  static bool Check(const Problem& p, Solution& s) {
    bool b = false;
    b = AdjusterMaxVolume::Check(p, s) || b;
    b = AdjusterAssignment::Check(p, s) || b;
    b = AdjusterSwaps::Check(p, s) || b;
    return b;
  }
};
