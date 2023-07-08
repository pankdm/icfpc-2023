#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/optimization/minimum.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

class OneMusucian {
 public:
  D2Point pos;
  double score;

 public:
  static bool FindBestLocationEarlyStop_IB(
      const Problem& p, unsigned instrument, const D2Point& start_point,
      const std::vector<OneMusucian>& current_best, OneMusucian& output,
      bool print_log = false, unsigned iteration = 0) {
    if (print_log) {
      std::cout << "Initial point for iteration " << iteration
                << " is: " << start_point.x << "\t" << start_point.y
                << std::endl;
    }
    double dlock = musician_collision_radius * musician_collision_radius;
    auto pos = start_point;
    auto score = Evaluator::DScoreIgnoreBlockedMusician(p, instrument, pos);
    for (;;) {
      // Check if too close to current best
      for (auto& m : current_best) {
        if ((SquaredDistanceL2(pos, m.pos) < dlock) && (score < m.score))
          return false;
      }
      // Find direction
      D2Vector v;
      for (auto& a : p.attendees) {
        D2Vector v0 = a.position - pos;
        auto v0ls = v0.LengthSquared();
        v += v0 * (score_mult * a.tastes[instrument] / (v0ls * v0ls));
      }
      if ((pos.x == p.stage.p1.x) && (v.dx < 0)) v.dx = 0.0;
      if ((pos.x == p.stage.p2.x) && (v.dx > 0)) v.dx = 0.0;
      if ((pos.y == p.stage.p1.y) && (v.dy < 0)) v.dy = 0.0;
      if ((pos.y == p.stage.p2.y) && (v.dy > 0)) v.dy = 0.0;
      if (v.LengthSquared() < 1e-12) {
        output.pos = pos;
        output.score = score;
        if (print_log) {
          std::cout << "Iteration " << iteration
                    << " stopped because of zero gradient. Final point:\n\t"
                    << score << "\t" << pos.x << "\t" << pos.y << std::endl;
        }
        return true;
      }
      double max_m = 1e6;
      if (v.dx > 0) max_m = std::min(max_m, (p.stage.p2.x - pos.x) / v.dx);
      if (v.dx < 0) max_m = std::min(max_m, (p.stage.p1.x - pos.x) / v.dx);
      if (v.dy > 0) max_m = std::min(max_m, (p.stage.p2.y - pos.y) / v.dy);
      if (v.dy < 0) max_m = std::min(max_m, (p.stage.p1.y - pos.y) / v.dy);
      auto f = [&](double x) {
        return -Evaluator::DScoreIgnoreBlockedMusician(p, instrument,
                                                       pos + v * x);
      };
      auto x0 = opt::Minimum(f, 0, max_m, 1e-6);
      pos += v * x0;
      auto new_score =
          Evaluator::DScoreIgnoreBlockedMusician(p, instrument, pos);
      // if (print_log)
      //   std::cout << "\t" << score << " -> " << new_score << std::endl;
      if (new_score <= score + 1e-6) {
        output.pos = pos;
        output.score = new_score;
        if (print_log) {
          std::cout << "Iteration " << iteration
                    << " stopped because of no improvement. Final point:\n\t"
                    << new_score << "\t" << pos.x << "\t" << pos.y << std::endl;
        }
        return true;
      }
      score = new_score;
    }
  }

  static std::vector<OneMusucian> FindBestLocations_IB(const Problem& p,
                                                       unsigned instrument,
                                                       unsigned max_iterations,
                                                       bool print_log = false) {
    double dx = (sqrt(5.0) - 1.0) / 2.0;
    double dy = sqrt(2.0) - 1.0;
    OneMusucian m;
    std::vector<OneMusucian> v;
    for (unsigned iteration = 0; iteration < max_iterations; ++iteration) {
      auto xi = dx * iteration, yi = dy * iteration;
      xi -= floor(xi);
      yi -= floor(yi);
      auto xf = (p.stage.p1.x != p.stage.p2.x)
                    ? p.stage.p1.x * xi + p.stage.p2.x * (1 - xi)
                    : p.stage.p1.x;
      auto yf = (p.stage.p1.y != p.stage.p2.y)
                    ? p.stage.p1.y * yi + p.stage.p2.y * (1 - yi)
                    : p.stage.p1.y;
      bool b = FindBestLocationEarlyStop_IB(p, instrument, {xf, yf}, v, m,
                                            print_log, iteration);
      bool skip = false;
      if (b) {
        for (unsigned i = 0; i < v.size(); ++i) {
          if (SquaredDistanceL2(v[i].pos, m.pos) <
              musician_collision_radius * musician_collision_radius) {
            if (m.score <= v[i].score) {
              skip = true;
              break;
              ;
            } else {
              std::swap(v[i--], v.back());
              v.pop_back();
            }
          }
        }
        if (!skip) v.push_back(m);
      }
    }
    return v;
  }
};
