#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "solvers/base.h"
#include "utils/one_musician.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solver.h"
#include "common/timer.h"

#include <algorithm>
#include <vector>

class Greedy3 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 protected:
  unsigned samples_per_search = 10;

 public:
  Greedy3() : BaseSolver() {}

  explicit Greedy3(unsigned _max_time, unsigned extra)
      : BaseSolver(_max_time), samples_per_search(extra) {
    if (samples_per_search == 0) samples_per_search = 10;
  }

  PSolver Clone() const override { return std::make_shared<Greedy3>(*this); }

  std::string Name() const override { return "loks_greedy3"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Timer t;
    const double dx = (sqrt(5.0) - 1.0) / 2.0;
    const double dy = sqrt(2.0) - 1.0;
    unsigned iteration = 0;
    OneMusucian md{{}, -1e16};
    std::vector<OneMusucian> cur_best(p.total_instruments, md);
    Solution s;
    s.SetId(p.Id());
    s.positions.resize(p.instruments.size(), D2Point{0., 0.});
    s.SetMaxVolume();
    std::vector<unsigned> vic(p.total_instruments, 0);
    std::vector<D2Point> vm;
    bool all_found = true;
    for (unsigned k = 0; k < p.instruments.size(); ++k) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        all_found = false;
        break;
      }
      double best = md.score;
      unsigned best_i = p.total_instruments;
      for (unsigned i = 0; i < p.total_instruments; ++i) {
        if (t.GetSeconds() > max_time_in_seconds) break;
        if (vic[i] == p.musicians[i].size()) continue;
        for (unsigned j = 0; j < samples_per_search; ++j) {
          auto xi = dx * iteration, yi = dy * iteration;
          xi -= floor(xi);
          yi -= floor(yi);
          auto xf = (p.stage.p1.x != p.stage.p2.x)
                        ? p.stage.p1.x * xi + p.stage.p2.x * (1 - xi)
                        : p.stage.p1.x;
          auto yf = (p.stage.p1.y != p.stage.p2.y)
                        ? p.stage.p1.y * yi + p.stage.p2.y * (1 - yi)
                        : p.stage.p1.y;
          if (j < 4) {
            xf = (iteration & 1) ? p.stage.p2.x : p.stage.p1.x;
            yf = (iteration < 2) ? p.stage.p1.y : p.stage.p2.y;
          }
          OneMusucian::FindBestLocationEarlyStop3_PB(
              p, i, {xf, yf}, vm, cur_best[i], false, iteration++);
        }
        if (cur_best[i].score > best) {
          best = cur_best[i].score;
          best_i = i;
        }
      }
      if (best_i == p.total_instruments) {
        // Can't insert any good candidate
        all_found = false;
        break;
      }
      auto m = cur_best[best_i];
      vm.push_back(m.pos);
      s.positions[p.musicians[best_i][vic[best_i]++]] = m.pos;
      // Invalidate all cur_best
      std::fill(cur_best.begin(), cur_best.end(), md);
    }
    if (all_found) {
      std::cout << "Greedy3:\t" << p.Id() << "\t" << t.GetSeconds() << "\t"
                << Evaluator::DScoreIgnoreBlockedNoBoost(p, s) << "\t"
                << Evaluator::DScore(p, s) << "\t" << Evaluator::IScore(p, s)
                << std::endl;
    } else {
      std::cout << "Greedy3:\t" << p.Id() << "\t" << t.GetSeconds()
                << "\tfailed." << std::endl;
      s.Clear();
    }
    return s;
  }
};
