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

class Shaker : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  Shaker() : BaseSolver() {}
  explicit Shaker(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override { return std::make_shared<Shaker>(*this); }

  std::string Name() const override { return "dm_shaker"; }

  bool SkipSolutionRead() const override { return true; }
  bool SkipBest() const override { return true; }

  std::vector<D2Point> FindBorderCandidates(const TProblem& p,
                                            const TSolution& s) {
    std::vector<D2Point> output;
    return output;
  }

  Solution Solve(const TProblem& p) override {
    Timer t;
    // Algorithm:
    //  - Read existing solution from a folder
    //  - Go through the border and find candidates
    //  - Swap candidates with zero-s while this is beneficial

    TSolution s;
    const std::string solver_name = "loks_best";
    if (!s.Load(p.Id(), solver_name)) {
      std::cerr << "No solution found! " << solver_name << std::endl;
      exit(1);
    }
    const auto current_score = Evaluator::Apply(p, s).score;
    std::cout << "Loaded, current score = " << current_score << std::endl;

    // const auto candidates = FindBorderCandidates(p, s);
    // for (;;) {
    //   // NOTE: only first iteration for now
    //   for (const auto& position : candidates) {
    //   }
    //   s.positions[] = best_position;
    // }

    return s;
  }
};
