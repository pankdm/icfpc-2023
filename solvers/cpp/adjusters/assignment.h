#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/algorithm/assignment/hungarian.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/linear_algebra/matrix.h"

#include <vector>

class AdjusterAssignment {
 public:
  bool Check(const Problem& p, Solution& s) {
    if (!Evaluator::Valid(p, s)) return false;
    double old_score = 0.;
    std::vector<std::vector<double>> vs(
        s.positions.size(), std::vector<double>(p.total_instruments, 0.));
    for (unsigned k = 0; k < s.positions.size(); ++k) {
      for (auto& a : p.attendees) {
        if (!Evaluator::Blocked(a, k, p, s)) {
          double d = score_mult / SquaredDistanceL2(a.position, s.positions[k]);
          for (unsigned i = 0; i < p.total_instruments; ++i)
            vs[k][i] += a.tastes[i] * d;
          old_score += a.tastes[p.instruments[k]] * d;
        }
      }
    }
    DMatrix m(s.positions.size());
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      for (unsigned j = 0; j < s.positions.size(); ++j)
        m(i, j) = -vs[j][p.instruments[i]];
    }
    alg::assignment::Hungarian h;
    auto new_score = -h.Solve(m, 1e16);
    // std::cout << "\t" << old_score << " -> " << new_score << std::endl;
    if (new_score > old_score) {
      Solution snew;
      snew.positions.resize(s.positions.size());
      auto a = h.Get();
      for (unsigned i = 0; i < snew.positions.size(); ++i)
        snew.positions[i] = s.positions[a[i]];
      s.positions.swap(snew.positions);
      return true;
    } else {
      return false;
    }
  }
};
