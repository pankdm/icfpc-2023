#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/algorithm/assignment/hungarian.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/linear_algebra/matrix.h"

#include <algorithm>
#include <vector>

class AdjusterAssignment {
 public:
  static bool Check(const Problem& p, Solution& s) {
    if (!Evaluator::Valid(p, s)) return false;
    int64_t old_iscore = 0.;
    std::vector<std::vector<int64_t>> vs(
        s.positions.size(), std::vector<int64_t>(p.total_instruments, 0));
    for (unsigned k = 0; k < s.positions.size(); ++k) {
      for (auto& a : p.attendees) {
        if (!Evaluator::Blocked(a, k, p, s)) {
          double d = score_mult / SquaredDistanceL2(a.position, s.positions[k]);
          for (unsigned i = 0; i < p.total_instruments; ++i)
            vs[k][i] += ceil(a.tastes[i] * d);
          old_iscore +=
              ceil(s.Volume(k) * ceil(a.tastes[p.instruments[k]] * d));
        }
      }
    }
    for (unsigned k = 0; k < vs.size(); ++k) {
      for (unsigned i = 0; i < vs[k].size(); ++i)
        vs[k][i] = std::max(vs[k][i], 0ll);
    }
    la::Matrix<int64_t> m(s.positions.size());
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      for (unsigned j = 0; j < s.positions.size(); ++j)
        m(i, j) = -vs[j][p.instruments[i]];
    }
    alg::assignment::Hungarian h;
    auto new_iscore = -h.Solve(m, (1ll << 20) * (1ll << 20)) * int(max_volume);
    std::cout << "\tExpected: " << old_iscore << " -> " << new_iscore
              << std::endl;
    if (new_iscore > old_iscore) {
      Solution snew;
      snew.positions.resize(s.positions.size());
      snew.SetMaxVolume();
      auto a = h.Get();
      for (unsigned i = 0; i < snew.positions.size(); ++i) {
        snew.positions[i] = s.positions[a[i]];
        if (vs[a[i]][p.instruments[i]] == 0) snew.volume[i] = 0.;
      }
      auto old_iscore2 = Evaluator::IScore(p, s),
           new_iscore2 = Evaluator::IScore(p, snew);
      std::cout << "\tFinal   : " << old_iscore2 << " -> " << new_iscore2
                << std::endl;
      if (new_iscore2 > old_iscore2) {
        s.positions.swap(snew.positions);
        return true;
      }
    }
    return false;
  }
};
