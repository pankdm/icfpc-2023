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
    double STEP = 5.;
    for (double x = p.stage.p1.x + 0.5 * (p.stage.p2.x - p.stage.p1.x);
         x < p.stage.p2.x; x += STEP) {
      output.push_back(D2Point{x, p.stage.p2.y});
    }
    for (double x = p.stage.p1.x + 0.5 * (p.stage.p2.x - p.stage.p1.x);
         x > p.stage.p1.x; x -= STEP) {
      output.push_back(D2Point{x, p.stage.p2.y});
    }

    for (double x = p.stage.p1.x + 0.5 * (p.stage.p2.x - p.stage.p1.x);
         x < p.stage.p2.x; x += STEP) {
      output.push_back(D2Point{x, p.stage.p1.y});
    }
    for (double x = p.stage.p1.x + 0.5 * (p.stage.p2.x - p.stage.p1.x);
         x > p.stage.p1.x; x -= STEP) {
      output.push_back(D2Point{x, p.stage.p1.y});
    }

    for (double y = p.stage.p1.y; y < p.stage.p2.y; y += STEP) {
      output.push_back(D2Point{p.stage.p1.x, y});
    }
    for (double y = p.stage.p1.y; y < p.stage.p2.y; y += STEP) {
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
    std::cout << "Starting " << Name() << " on problem " << p.Id() << std::endl;
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
    auto start = t.GetMilliseconds();
    auto start_score = Evaluator::Apply(p, s).score;
    auto current_score = start_score;
    std::cout << "Loaded, current score = " << start_score
              << ", time = " << t.GetMilliseconds() - start << "ms"
              << std::endl;

    std::vector<std::pair<double, int>> musician_scores(s.positions.size());
    for (int i = 0; i < musician_scores.size(); ++i) {
      musician_scores[i] =
          std::make_pair(Evaluator::IScoreMusician(p, i, s, 1), i);
    }
    sort(begin(musician_scores), end(musician_scores));
    std::vector<bool> updated(s.positions.size(), false);

    const auto candidates = FindBorderCandidates(p, s);

    int candidate_idx = 0;

    for (int iter = 0; iter < 100; ++iter) {
      auto candidate_position = D2Point{-1, -1};
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

      int best_index = -1;
      double best_score = -1;
      for (const auto& pr : musician_scores) {
        if (pr.first > 0) {
          break;
        }
        int m_idx = pr.second;
        if (updated[m_idx]) {
          continue;
        }
        // std::cout << "music at " << s.positions[m_idx].x << " "
        //           << s.positions[m_idx].y << std::endl;
        // auto old_position = s.positions[m_idx];
        // s.positions[m_idx] = candidate_position;
        // double score = Evaluator::IScoreMusician(p, m_idx, s, 1);
        // s.positions[m_idx] = old_position;
        double score = 0;
        if (best_index == -1) {
          best_index = m_idx;
          best_score = score;
          std::cout << "  " << pr.second << " -> " << pr.first
                    << " score = " << score << std::endl;
          break;
        }
      }

      if (best_index >= 0) {
        std::cout << ".. Setting " << best_index << " to "
                  << candidate_position.x << ", " << candidate_position.y
                  << std::endl;
        s.positions[best_index] = candidate_position;
        updated[best_index] = true;

      } else {
        std::cout << "not found next musician" << std::endl;
        break;
      }

      auto res = Evaluator::Apply(p, s);
      if (res.correct) {
        std::cout << "Finished iteration, current score = " << res.score
                  << ", time = " << t.GetMilliseconds() - start << "ms"
                  << std::endl;
      }
    }

    auto res = Evaluator::Apply(p, s);
    if (res.correct) {
      std::cout << "Finished, current score = " << res.score
                << ", time = " << t.GetMilliseconds() - start << "ms"
                << std::endl;
    } else {
      std::cout << "Invalid solution!" << std::endl;
    }
    auto snew = s;
    std::cout << " Started assignment run for problem " << p.Id() << std::endl;
    AdjusterAssignment adj;
    if (adj.Check(p, snew)) {
      auto score_old = Evaluator::Apply(p, s).score;
      auto score_new = Evaluator::Apply(p, snew).score;
      std::cout << "New solution from adjuster for problem " << p.Id() << ":\t"
                << start_score << " -> " << score_old << " -> " << score_new
                << std::endl;
      s = snew;
    }

    s.Save(Name());
    std::cout << "  ..saving solution to " << Name() << std::endl;

    return s;
  }
};
