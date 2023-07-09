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
#include <iostream>
#include <vector>

class BorderSolver : public BaseSolver {
 public:
  using TBase = BaseSolver;
  using PSolver = TBase::PSolver;

 public:
  BorderSolver() : BaseSolver() {}
  explicit BorderSolver(unsigned _max_time) : BaseSolver(_max_time) {}

  PSolver Clone() const override {
    return std::make_shared<BorderSolver>(*this);
  }

  std::string Name() const override { return "dm_border"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  std::vector<D2Point> FindBorderCandidates(const TProblem& p,
                                            const TSolution& s) {
    std::vector<D2Point> output;
    double STEP = 10.;
    for (double x = p.stage.p1.x; x <= p.stage.p2.x; x += STEP) {
      output.push_back(D2Point{x, p.stage.p1.y});
    }
    for (double x = p.stage.p1.x; x <= p.stage.p2.x; x += STEP) {
      output.push_back(D2Point{x, p.stage.p2.y});
    }
    for (double y = p.stage.p1.y; y <= p.stage.p2.y; y += STEP) {
      output.push_back(D2Point{p.stage.p1.x, y});
    }
    for (double y = p.stage.p1.y; y <= p.stage.p2.y; y += STEP) {
      output.push_back(D2Point{p.stage.p2.x, y});
    }
    return output;
  }

  bool CheckNonOverlapping(const D2Point& pi, const TProblem& p,
                           const TSolution& s) {
    for (unsigned j = 0; j < s.positions.size(); ++j) {
      if (SquaredDistanceL2(pi, s.positions[j]) <
          musician_collision_radius * musician_collision_radius) {
        return false;
      }
    }
    return true;
  }

  Solution Solve(const TProblem& p) override {
    Timer t;
    TSolution s;

    // drop if perimeter is too large
    int border_count =
        (2 * ((p.stage.p2.x - p.stage.p1.x) + (p.stage.p2.y - p.stage.p1.y))) /
        musician_collision_radius;
    if (p.instruments.size() < border_count) {
      std::cout << "Skipping problem " << p.Id() << std::endl;
      return s;
    }

    std::cout << "Starting " << Name() << " on problem " << p.Id() << std::endl;

    TSolution best_s;
    const std::string solver_name = "loks_best";
    if (!best_s.Load(p.Id(), solver_name)) {
      std::cerr << "No best solution found! " << solver_name << std::endl;
      exit(1);
    }
    auto start = t.GetMilliseconds();
    auto start_score = Evaluator::Apply(p, best_s).score;
    // auto current_score = start_score;
    std::cout << "Loaded, current score = " << start_score
              << ", time = " << t.GetMilliseconds() - start << "ms"
              << std::endl;

    s.SetId(p.Id());
    s.positions.resize(p.instruments.size());
    const auto candidates = FindBorderCandidates(p, s);
    int candidate_idx = 0;
    int m_idx = 0;
    for (; m_idx < s.positions.size(); ++m_idx) {
      auto candidate_position = D2Point{-50, -50};
      bool found = false;
      for (; candidate_idx < candidates.size(); ++candidate_idx) {
        if (CheckNonOverlapping(candidates[candidate_idx], p, s)) {
          found = true;
          candidate_position = candidates[candidate_idx];
          break;
        }
      }
      if (!found) {
        std::cout << "not found next candidate until " << candidate_idx
                  << std::endl;
        break;
      }
      s.positions[m_idx] = candidate_position;
    }

    // auto res = Evaluator::Apply(p, s);
    // if (res.correct) {
    //   std::cout << "Finished iteration, current score = " << res.score
    //             << ", time = " << t.GetMilliseconds() - start << "ms"
    //             << std::endl;
    // }

    std::cout << "filling out remaining" << std::endl;
    auto pos = D2Point{p.stage.p1.x + musician_collision_radius,
                       p.stage.p1.y + musician_collision_radius};
    for (; m_idx < s.positions.size(); ++m_idx) {
      for (;;) {
        if (CheckNonOverlapping(pos, p, s)) {
          s.positions[m_idx] = pos;
          break;
        }
        pos.x += musician_collision_radius;
        if (pos.x > p.stage.p2.x) {
          pos.x = p.stage.p1.x;
          pos.y += musician_collision_radius;
          if (pos.y > p.stage.p2.y) {
            std::cout << "Error filling musicians" << std::endl;
            break;
          }
        }
      }
    }

    auto snew = s;
    std::cout << " Started assignment run for problem " << p.Id() << std::endl;
    AdjusterAssignment adj;
    if (adj.Check(p, snew)) {
      auto score_new = Evaluator::Apply(p, snew).score;
      std::cout << "New solution from adjuster for problem " << p.Id() << ":\t"
                << start_score << " -> " << score_new << std::endl;
      s = snew;
    }

    s.Save(Name());
    std::cout << "  ..saving solution to " << Name() << std::endl;

    return s;
  }
};
