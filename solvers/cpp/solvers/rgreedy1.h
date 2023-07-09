#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "solvers/base.h"
#include "utils/one_musician2.h"

#include "common/data_structures/unsigned_set.h"
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

  std::string Name() const override { return "loks_rgreedy1"; }

  bool SkipSolutionRead() const override { return true; }
  // bool SkipBest() const override { return true; }

  Solution Solve(const TProblem& p) override {
    Timer t;
    const bool boost_enabled = !p.Lighting();
    const double dx = (sqrt(5.0) - 1.0) / 2.0;
    const double dy = sqrt(2.0) - 1.0;
    const double d2 = musician_collision_radius * musician_collision_radius;
    unsigned iteration = 0;
    Solution s;
    s.SetId(p.Id());
    s.positions.resize(p.instruments.size(), D2Point{0., 0.});
    s.SetMaxVolume();
    std::vector<unsigned> vic(p.total_instruments, 0);
    std::vector<OneMusician2> vm;
    std::vector<double> vscore_without_boost(p.total_instruments);
    ds::UnsignedSet us_instruments(p.total_instruments);
    us_instruments.InsertAll();

    bool all_found = true;
    double expected_score = 0.;
    for (unsigned k = 0; k < p.instruments.size(); ++k) {
      if (t.GetSeconds() > max_time_in_seconds) {
        // Time to stop
        all_found = false;
        break;
      }
      double best = -1e16;
      unsigned best_i = p.total_instruments;
      D2Point best_p;
      auto usl = us_instruments.List();
      for (unsigned j = 0; j < samples_per_search; ++j, ++iteration) {
        if (t.GetSeconds() > max_time_in_seconds) break;
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
          if (SquaredDistanceL2(ptest, m.pos) < d2) {
            collide = true;
            break;
          }
        }
        if (collide) continue;
        double score_common = 0;
        for (auto& m : vm) {
          auto v =
              OneMusician2::FindBlockedAudience(m.audience, m.pos, ptest, p);
          m.delta_score_without_boost = 0.;
          for (auto i : v) {
            auto& a = p.attendees[i];
            m.delta_score_without_boost -= score_mult * a.tastes[m.instrument] /
                                           SquaredDistanceL2(a.position, m.pos);
          }
          auto ds =
              std::max(m.score_without_boost + m.delta_score_without_boost,
                       0.) -
              std::max(m.score_without_boost, 0.);
          score_common += m.boost_mult * ds;
        }
        auto va = OneMusician2::FindAudience(ptest, p, vm);
        std::fill(vscore_without_boost.begin(), vscore_without_boost.end(), 0.);
        for (auto i : va) {
          auto& a = p.attendees[i];
          double d = score_mult / SquaredDistanceL2(a.position, ptest);
          for (auto j : usl) vscore_without_boost[j] += a.tastes[j] * d;
        }
        for (auto j : usl) {
          double score = score_common;
          if (boost_enabled) {
            double boost = 1.0;
            for (auto& m : vm) {
              if (m.instrument == j) {
                double db = 1.0 / DistanceL2(m.pos, ptest);
                boost += db;
                score += db * std::max(m.score_without_boost +
                                           m.delta_score_without_boost,
                                       0.);
              }
            }
            score += boost * std::max(vscore_without_boost[j], 0.);
          } else {
            score += std::max(vscore_without_boost[j], 0.);
          }
          if (score > best) {
            best = score;
            best_i = j;
            best_p = ptest;
          }
        }
      }

      for (unsigned i = 0; i < p.total_instruments; ++i) {
        if (vic[i] == p.musicians[i].size()) continue;
      }
      if (best_i == p.total_instruments) {
        // Can't insert any good candidate
        all_found = false;
        break;
      }
      expected_score += best;
      assert(vic[best_i] < p.musicians[best_i].size());
      unsigned sindex = p.musicians[best_i][vic[best_i]++];
      s.positions[sindex] = best_p;
      if (vic[best_i] == p.musicians[best_i].size())
        us_instruments.Remove(best_i);
      // Init new musician
      OneMusician2 mnew;
      mnew.pos = best_p;
      mnew.instrument = best_i;
      mnew.sindex = sindex;
      mnew.audience = OneMusician2::FindAudience(mnew.pos, p, vm);
      mnew.score_without_boost = 0.;
      for (auto i : mnew.audience) {
        auto& a = p.attendees[i];
        mnew.score_without_boost += a.tastes[mnew.instrument] * score_mult /
                                    SquaredDistanceL2(a.position, mnew.pos);
      }
      mnew.boost_mult = 1.0;
      // Adjust audience for old ones
      for (auto& m : vm) {
        auto v =
            OneMusician2::FindBlockedAudience(m.audience, m.pos, mnew.pos, p);
        if (v.empty()) continue;
        std::sort(v.begin(), v.end());
        for (auto i : v) {
          auto& a = p.attendees[i];
          m.score_without_boost -= score_mult * a.tastes[m.instrument] /
                                   SquaredDistanceL2(a.position, m.pos);
        }
        for (unsigned i = 0; i < m.audience.size(); ++i) {
          if (std::binary_search(v.begin(), v.end(), m.audience[i])) {
            m.audience[i--] = m.audience.back();
            m.audience.pop_back();
          }
        }
      }
      if (boost_enabled) {
        for (auto& m : vm) {
          if (m.instrument == mnew.instrument) {
            double db = 1. / DistanceL2(m.pos, mnew.pos);
            m.boost_mult += db;
            mnew.boost_mult += db;
          }
        }
      }
      vm.push_back(mnew);
    }
    if (all_found) {
      for (auto& m : vm) {
        if (m.score_without_boost <= 0.) s.volume[m.sindex] = 0.;
      }
      std::cout << "RGreedy1:\t" << p.Id() << "\t" << t.GetSeconds() << "\t"
                << max_volume * expected_score << "\t"
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
