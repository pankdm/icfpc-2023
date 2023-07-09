#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "solvers/base.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solver.h"
#include "common/timer.h"

#include <algorithm>
#include <vector>

class RGreedy1 : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 protected:
  unsigned samples_per_search = 100;

 public:
  RGreedy1() : BaseSolver() {}

  explicit RGreedy1(unsigned _max_time, unsigned extra)
      : BaseSolver(_max_time), samples_per_search(extra) {
    if (samples_per_search == 0) samples_per_search = 100;
  }

  PSolver Clone() const override { return std::make_shared<RGreedy1>(*this); }

  std::string Name() const override { return "loks_greedy1"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Timer t;
    const double dx = (sqrt(5.0) - 1.0) / 2.0;
    const double dy = sqrt(2.0) - 1.0;
    const double d2 = musician_collision_radius * musician_collision_radius;
    unsigned iteration = 0;
    Solution s;
    s.SetId(p.Id());
    s.positions.resize(p.instruments.size(), D2Point{0., 0.});
    std::vector<unsigned> vic(p.total_instruments, 0);
    std::vector<D2Point> vm;
    bool all_found = true;
    for (unsigned k = 0; k < p.instruments.size(); ++k) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        all_found = false;
        break;
      }
      double best = -1e16;
      unsigned best_i = p.total_instruments;
      D2Point best_p;
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
          // For safety
          xf = std::max(p.stage.p1.x, std::min(xf, p.stage.p2.x));
          yf = std::max(p.stage.p1.y, std::min(yf, p.stage.p2.y));
          D2Point ptest(xf, yf);
          bool collide = false;
          for (auto& m : vm) {
            if (SquaredDistanceL2(ptest, m) < d2) {
              collide = true;
              break;
            }
          }
          if (collide) continue;
          // ...
          double score = 0.;
          if (score > best) {
            best = score;
            best_i = i;
            best_p = ptest;
          }
        }
      }
      if (best_i == p.total_instruments) {
        // Can't insert any good candidate
        all_found = false;
        break;
      }
      vm.push_back(best_p);
      s.positions[p.musicians[best_i][vic[best_i]++]] = best_p;
    }
    if (all_found) {
      std::cout << "RGreedy1:\t" << p.Id() << "\t" << t.GetSeconds() << "\t"
                << Evaluator::DScoreIgnoreBlockedNoBoost(p, s) << "\t"
                << Evaluator::DScore(p, s) << "\t" << Evaluator::IScore(p, s)
                << std::endl;
    } else {
      std::cout << "RGreedy1:\t" << p.Id() << "\t" << t.GetSeconds()
                << "\tfailed." << std::endl;
      s.Clear();
    }
    return s;
  }
};
