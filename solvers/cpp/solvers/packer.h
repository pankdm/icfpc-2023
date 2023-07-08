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

class Packer : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Packer() : BaseSolver() {}
  explicit Packer(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Packer>(*this); }

  std::string Name() const override { return "dm_packer"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  bool CheckNonOverlapping(const std::vector<D2Point>& candidates, D2Point pt) {
    const double d2 = musician_collision_radius * musician_collision_radius;
    for (const auto& p : candidates) {
      if (SquaredDistanceL2(p, pt) < d2) {
        return false;
      }
    }
    return true;
  }

  Solution Solve(const TProblem& p) override {
    Timer t;

    Solution s;
    s.SetId(p.Id());
    s.positions.resize(p.instruments.size(), D2Point{0., 0.});

    std::vector<D2Point> candidates;
    double y = p.stage.p2.y;
    while (y >= p.stage.p1.y) {
      // candidates.push_back(D2Point{p.stage.p1.x, y});
      candidates.push_back(D2Point{p.stage.p2.x, y});
      y -= musician_collision_radius;
    }

    double x = p.stage.p2.x;
    while (x >= p.stage.p2.y) {
      candidates.push_back(D2Point{x, p.stage.p2.y});
      x -= musician_collision_radius;
    }

    auto cur = p.stage.p1;
    while (true) {
      if (candidates.size() >= s.positions.size()) {
        break;
      }
      while (true) {
        if (CheckNonOverlapping(candidates, cur)) {
          candidates.push_back(cur);
          break;
        }
        cur.x += musician_collision_radius;
        if (cur.x > p.stage.p2.x) {
          cur.x = 0;
          cur.y += musician_collision_radius;
        }
      }
    }

    for (int i = 0; i < s.positions.size(); ++i) {
      s.positions[i] = candidates[i];
    }
    auto snew = s;
    std::cout << " Started assignment run for problem " << p.Id() << std::endl;
    AdjusterAssignment adj;
    if (adj.Check(p, snew)) {
      auto score_old = Evaluator::Apply(p, s).score;
      auto score_new = Evaluator::Apply(p, snew).score;
      std::cout << "New solution from adjuster for problem " << p.Id() << ":\t"
                << score_old << " -> " << score_new << std::endl;
      s = snew;
    }


    bool all_found = true;
    if (all_found) {
      std::cout << "Packer:\t" << p.Id() << "\t" << t.GetSeconds() << "\t"
                << Evaluator::DScoreIgnoreBlocked(p, s) << "\t"
                << Evaluator::DScore(p, s) << "\t" << Evaluator::IScore(p, s)
                << std::endl;
    } else {
      std::cout << "Packer:\t" << p.Id() << "\t" << t.GetSeconds()
                << "\tfailed." << std::endl;
      s.Clear();
    }
    return s;
  }
};
