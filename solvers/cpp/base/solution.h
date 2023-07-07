#pragma once

#include "common/geometry/d2/point.h"
#include "common/solvers/solution.h"

#include <vector>

class Solution : public solvers::Solution {
 public:
  std::vector<D2Point> positions;
};
