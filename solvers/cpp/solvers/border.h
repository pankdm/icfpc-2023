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
  explicit BorderSolver(unsigned _max_time, int layers)
      : BaseSolver(_max_time), _layers(layers) {}

  PSolver Clone() const override {
    return std::make_shared<BorderSolver>(*this);
  }
  int _layers;

  std::string Name() const override { return "dm_border2"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  std::vector<D2Point> FindBorderCandidates(const TProblem& p,
                                            const TSolution& s, double layers) {
    std::vector<D2Point> output;
    auto fill_left = p.stage.p1.x > musician_collision_radius;
    auto fill_bottom = p.stage.p1.y > musician_collision_radius;
    std::cout << "stage: " << p.stage.p1.x << " " << p.stage.p1.y << std::endl;
    if (fill_left && fill_bottom) {
      output.push_back(D2Point{p.stage.p1.x, p.stage.p1.y});
    }
    output.push_back(D2Point{p.stage.p1.x, p.stage.p2.y});
    output.push_back(D2Point{p.stage.p2.x, p.stage.p1.y});
    output.push_back(D2Point{p.stage.p2.x, p.stage.p2.y});

    int LAYERS = layers;
    double STEP = 10.0;
    double HALF_STEP = STEP * 0.5;
    double OFFSET = sqrt(100 - HALF_STEP * HALF_STEP) + 0.01;
    {
      for (int layer = 0; layer < LAYERS; ++layer) {
        double START_STEP = HALF_STEP * (layer % 2);
        double LAYER_OFFSET = OFFSET * layer;
        // double STEP = 11.;
        if (fill_left) {
          for (double x = p.stage.p1.x + START_STEP; x <= p.stage.p2.x;
               x += STEP) {
            output.push_back(D2Point{x, p.stage.p1.y + LAYER_OFFSET});
          }
        }
        for (double x = p.stage.p1.x + START_STEP; x <= p.stage.p2.x;
             x += STEP) {
          output.push_back(D2Point{x, p.stage.p2.y - LAYER_OFFSET});
        }
        if (fill_bottom) {
          for (double y = p.stage.p1.y + START_STEP; y <= p.stage.p2.y;
               y += STEP) {
            output.push_back(D2Point{p.stage.p1.x + LAYER_OFFSET, y});
          }
        }
        for (double y = p.stage.p1.y + START_STEP; y <= p.stage.p2.y;
             y += STEP) {
          output.push_back(D2Point{p.stage.p2.x - LAYER_OFFSET, y});
        }
      }
    }

    return output;
  }

  bool CheckNonOverlapping(const D2Point& pi, const TSolution& s) {
    for (unsigned j = 0; j < s.positions.size(); ++j) {
      if (SquaredDistanceL2(pi, s.positions[j]) <
          musician_collision_radius * musician_collision_radius) {
        return false;
      }
    }
    return true;
  }

  double SolveWithLayers(const TProblem& p0, TSolution& s, double layers) {
    auto p = p0;
    Timer t;
    auto start = t.GetMilliseconds();
    // auto start_score = Evaluator::Apply(p, best_s).score;
    // auto current_score = start_score;
    // std::cout << "Loaded, current score = " << start_score
    //           << ", time = " << t.GetMilliseconds() - start << "ms"
    //           << std::endl;

    s.SetId(p.Id());
    s.positions.resize(p.instruments.size());
    s.SetMaxVolume();
    const auto candidates = FindBorderCandidates(p, s, layers);

    int candidate_idx = 0;
    int m_idx = 0;
    for (; m_idx < s.positions.size(); ++m_idx) {
      auto candidate_position = D2Point{-50, -50};
      bool found = false;
      for (; candidate_idx < candidates.size(); ++candidate_idx) {
        if (CheckNonOverlapping(candidates[candidate_idx], s)) {
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

    int added = 0;
    {
      if (candidate_idx < candidates.size()) {
        if (m_idx != s.positions.size()) {
          std::cout << "ERROR!" << std::endl;
          exit(1);
        }
        // Add fake instruments
        int fake_ins = p.total_instruments;
        p.total_instruments += 1;
        for (auto& a : p.attendees) {
          a.tastes.push_back(0.0);
        }
        std::vector<unsigned> fake_mapping;

        for (; candidate_idx < candidates.size(); ++candidate_idx) {
          if (CheckNonOverlapping(candidates[candidate_idx], s)) {
            auto candidate_position = candidates[candidate_idx];

            fake_mapping.push_back(s.positions.size());
            s.positions.push_back(candidate_position);
            p.instruments.push_back(fake_ins);
            m_idx += 1;
            added += 1;
          }
        }
        p.musicians.push_back(fake_mapping);
      }
    }
    std::cout << "Added " << added << " fake musicians" << std::endl;

    // auto res = Evaluator::Apply(p, s);
    // if (res.correct) {
    //   std::cout << "Finished iteration, current score = " << res.score
    //             << ", time = " << t.GetMilliseconds() - start << "ms"
    //             << std::endl;
    // }

    // std::cout << "filling out remaining" << std::endl;
    auto pos = D2Point{p.stage.p1.x + musician_collision_radius,
                       p.stage.p1.y + musician_collision_radius};
    for (; m_idx < s.positions.size(); ++m_idx) {
      for (;;) {
        if (CheckNonOverlapping(pos, s)) {
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

    s.SetMaxVolume();

    auto snew = s;
    // std::cout << " Started assignment run for problem " << p.Id() <<
    // std::endl;
    AdjusterAssignment adj;
    if (adj.Check(p, snew)) {
      auto score_new = Evaluator::Apply(p, snew).score;
      // std::cout << "New solution from adjuster for problem " << p.Id() <<
      // ":\t"
      //           << start_score << " -> " << score_new << std::endl;

      std::cout << "  New solution from Assigner for problem " << p.Id()
                << ":\t" << score_new
                << ", time = " << t.GetMilliseconds() - start << "ms"
                << std::endl;
      AdjusterSwaps adj2;
      adj2.Check(p, snew);
      auto score2 = Evaluator::Apply(p, snew).score;

      std::cout << "  New solution from Swapper for problem " << p.Id() << ":\t"
                << score2 << ", time = " << t.GetMilliseconds() - start << "ms"
                << std::endl;

      s = snew;
      s.positions.resize(s.positions.size() - added);
      s.SetMaxVolume();
      // s.Save(Name());
      // std::cout << "  ..saving solution to " << Name() << std::endl;
      return score2;
    }
    return -1;
  }

  Solution Solve(const TProblem& p) override {
    Timer t;

    // // drop if perimeter is too large
    // int border_count =
    //     (2 * ((p.stage.p2.x - p.stage.p1.x) + (p.stage.p2.y - p.stage.p1.y)))
    //     / musician_collision_radius;
    // if (p.instruments.size() < border_count) {
    //   std::cout << "Skipping problem " << p.Id() << " " <<
    //   p.instruments.size()
    //             << " vs " << border_count << std::endl;
    //   TSolution s;
    //   return s;
    // }

    std::cout << "Starting " << Name() << " on problem " << p.Id() << std::endl;

    TSolution best_s;
    const std::string solver_name = "loks_best";
    if (!best_s.Load(p.Id(), solver_name)) {
      std::cerr << "No best solution found! " << solver_name << std::endl;
      exit(1);
    }

    int64_t start_score = Evaluator::Apply(p, best_s).score;
    int64_t current_score = start_score;
    std::cout << "Loaded, current score = "
              << "\t" << start_score << ", time = " << t.GetMilliseconds()
              << "ms" << std::endl;

    TSolution s;
    int64_t end_score = SolveWithLayers(p, s, _layers);

    std::cout << "... >> problem " << p.Id() << "\n";
    std::cout << "... >>> before: \t" << start_score << "\n";
    std::cout << "... >>> after : \t" << end_score << std::endl;
    // double score = SolveWithStep(p, 10.0);

    std::cout << "..... problem " << p.Id() << " -> before: " << start_score
              << " -> after: " << end_score << std::endl;

    auto name_tmp = Name() + "_tmp";
    s.Save(name_tmp);
    std::cout << "  ..saving solution to " << name_tmp << std::endl;

    return s;
  }
};
