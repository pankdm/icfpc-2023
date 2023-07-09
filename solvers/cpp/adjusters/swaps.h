#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/timer.h"

#include <algorithm>
#include <vector>

class AdjusterSwaps {
 public:
  static bool Check(const Problem& p, Solution& s) {
    if (p.Lighting() || (p.instruments.size() == p.total_instruments)) {
      // AdjusterAssignment output optimal solution under the same constraints.
      return false;
    }
    Timer t;
    if (!Evaluator::Valid(p, s)) return false;
    double old_dscore = 0.;
    auto vb = Evaluator::ScoreBoost(p, s);
    std::vector<std::vector<double>> vs(
        s.positions.size(), std::vector<double>(p.total_instruments, 0.));
    for (unsigned k = 0; k < s.positions.size(); ++k) {
      for (auto& a : p.attendees) {
        if (!Evaluator::Blocked(a, k, p, s)) {
          double d = score_mult / SquaredDistanceL2(a.position, s.positions[k]);
          for (unsigned i = 0; i < p.total_instruments; ++i)
            vs[k][i] += a.tastes[i] * d;
          old_dscore += a.tastes[p.instruments[k]] * d * vb[k];
        }
      }
    }
    double new_dscore = old_dscore;
    Solution snew = s;
    std::vector<std::vector<double>> vvd(
        s.positions.size(), std::vector<double>(p.total_instruments, 0.));
    for (; t.GetSeconds() < 120;) {
      for (unsigned i = 0; i < snew.positions.size(); ++i) {
        for (unsigned j = 0; j < p.total_instruments; ++j) {
          auto& dscore = vvd[i][j];
          dscore = 0.;
          if (j != p.instruments[i]) {
            double dbj = 1.0;
            for (auto k : p.musicians[j]) {
              double db = 1. / DistanceL2(snew.positions[i], snew.positions[k]);
              dscore += db * vs[k][j];
              dbj += db;
            }
            dscore += dbj * vs[i][j];
          } else {
            // j == p.instruments[i]
            double dbj = 1.0;
            for (auto k : p.musicians[j]) {
              if (k == i) continue;
              double db = 1. / DistanceL2(snew.positions[i], snew.positions[k]);
              dscore -= db * vs[k][j];
              dbj += db;
            }
            dscore -= dbj * vs[i][j];
          }
        }
      }
      double best_delta = 0.;
      unsigned best_i = 0, best_j = 0;
      for (unsigned i = 0; i < snew.positions.size(); ++i) {
        for (unsigned j = 0; j < i; ++j) {
          if (p.instruments[i] == p.instruments[j]) continue;
          auto ii = p.instruments[i], ij = p.instruments[j];
          double db = 1. / DistanceL2(snew.positions[i], snew.positions[j]);
          double delta = vvd[i][ii] + vvd[i][ij] + vvd[j][ii] + vvd[j][ij] -
                         db * (vs[i][ii] + vs[i][ij] + vs[j][ii] + vs[j][ij]);
          if (best_delta < delta) {
            best_delta = delta;
            best_i = i;
            best_j = j;
          }
        }
      }
      if (best_delta == 0.) break;
      // std::cout << "New swap found:\t" << best_i << "\t" << best_j << "\t"
      //           << best_delta << std::endl;
      new_dscore += best_delta;
      std::swap(snew.positions[best_i], snew.positions[best_j]);
      vs[best_i].swap(vs[best_j]);
    }

    if (new_dscore > old_dscore) {
      std::cout << "\t" << old_dscore << " -> " << new_dscore << "\t"
                << Evaluator::DScore(p, snew) << std::endl;
      auto iscore_old = Evaluator::IScore(p, s),
           iscore_new = Evaluator::IScore(p, snew);
      std::cout << "\t\t" << iscore_old << " -> " << iscore_new << std::endl;
      if (iscore_new > iscore_old) {
        s.positions.swap(snew.positions);
        return true;
      }
    }
    return false;
  }
};
