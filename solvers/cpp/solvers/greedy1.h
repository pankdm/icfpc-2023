#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "utils/one_musician.h"

#include "common/geometry/d2/point.h"
#include "common/solvers/solver.h"

#include <algorithm>
#include <vector>

class Greedy1 : public solvers::Solver<Problem, Solution, Evaluator> {
 public:
  using TBase = solvers::Solver<Problem, Solution, Evaluator>;
  using PSolver = TBase::PSolver;

 public:
  PSolver Clone() const override { return std::shared_ptr<TSelf>(); }
  std::string Name() const override { return "loks_greedy1"; }

  Solution Solve(const TProblem& p) override {
    std::vector<std::vector<OneMusucian>> vvc(p.total_instruments);
    for (unsigned i = 0; i < p.total_instruments; ++i) {
      vvc[i] = OneMusucian::FindBestLocations_IB(p, i, 1000);
      std::sort(vvc[i].begin(), vvc[i].end(),
                [](const OneMusucian& l, const OneMusucian& r) {
                  return l.score < r.score;
                });
    }
    Solution s;
    s.SetId(p.Id());
    s.positions.resize(p.instruments.size(), D2Point{0., 0.});
    std::vector<std::vector<unsigned>> imap(p.total_instruments);
    for (unsigned i = 0; i < p.instruments.size(); ++i)
      imap[p.instruments[i]].push_back(i);
    std::vector<unsigned> vic(p.total_instruments, 0);
    std::vector<OneMusucian> vm;
    double expected_dscore_ib = 0.;
    bool all_found = true;
    for (unsigned k = 0; k < p.instruments.size(); ++k) {
      double best = -1e16;
      unsigned best_i = p.total_instruments;
      for (unsigned i = 0; i < p.total_instruments; ++i) {
        if ((vic[i] == imap[i].size()) || vvc[i].empty()) continue;
        for (; !vvc[i].empty() && (vvc[i].back().score > best);) {
          auto& mi = vvc[i].back();
          bool ok = true;
          for (auto& m : vm) {
            if (SquaredDistanceL2(m.pos, mi.pos) <
                musician_collision_radius * musician_collision_radius) {
              ok = false;
              break;
            }
          }
          if (ok) {
            best = mi.score;
            best_i = i;
            break;
          } else {
            vvc[i].pop_back();
          }
        }
      }
      if (best_i == p.total_instruments) {
        // Can't insert any good candidate
        all_found = false;
        break;
      }
      auto m = vvc[best_i].back();
      vm.push_back(m);
      s.positions[imap[best_i][vic[best_i]++]] = m.pos;
      expected_dscore_ib += best;
      vvc[best_i].pop_back();
    }
    if (all_found) {
      std::cout << "Greedy1:\t" << p.Id() << "\t" << expected_dscore_ib << "\t"
                << Evaluator::DScoreIgnoreBlocked(p, s) << "\t"
                << Evaluator::DScore(p, s) << "\t" << Evaluator::IScore(p, s)
                << std::endl;
    }
    return s;
  }
};
