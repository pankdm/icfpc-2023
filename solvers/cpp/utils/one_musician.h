#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/segment.h"
#include "common/geometry/d2/utils/intersect_segment.h"
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
      // Project if on border
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
      if (iteration < 4) {
        xf = (iteration & 1) ? p.stage.p2.x : p.stage.p1.x;
        yf = (iteration < 2) ? p.stage.p1.y : p.stage.p2.y;
      }
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

  static bool FindBestLocationEarlyStop2_IB(
      const Problem& p, unsigned instrument, const D2Point& start_point,
      const std::vector<D2Point>& selected, OneMusucian& current_best,
      bool print_log = false, unsigned iteration = 0) {
    if (print_log) {
      std::cout << "Initial point for iteration " << iteration
                << " is: " << start_point.x << "\t" << start_point.y
                << std::endl;
    }
    double dlock = musician_collision_radius * musician_collision_radius;
    auto pos = start_point;
    for (auto& m : selected) {
      if (SquaredDistanceL2(pos, m) < dlock) return false;
    }
    auto score = Evaluator::DScoreIgnoreBlockedMusician(p, instrument, pos);

    auto UpdateBest = [&]() {
      if (score > current_best.score) {
        current_best.score = score;
        current_best.pos = pos;
      }
    };

    for (;;) {
      // Check if too close to current best
      if ((SquaredDistanceL2(pos, current_best.pos) < dlock) &&
          (score < current_best.score))
        return false;
      // Find direction
      D2Vector v;
      for (auto& a : p.attendees) {
        D2Vector v0 = a.position - pos;
        auto v0ls = v0.LengthSquared();
        v += v0 * (score_mult * a.tastes[instrument] / (v0ls * v0ls));
      }
      // Project if on border
      if ((pos.x == p.stage.p1.x) && (v.dx < 0)) v.dx = 0.0;
      if ((pos.x == p.stage.p2.x) && (v.dx > 0)) v.dx = 0.0;
      if ((pos.y == p.stage.p1.y) && (v.dy < 0)) v.dy = 0.0;
      if ((pos.y == p.stage.p2.y) && (v.dy > 0)) v.dy = 0.0;
      if (v.LengthSquared() < 1e-12) {
        UpdateBest();
        if (print_log) {
          std::cout << "Iteration " << iteration
                    << " stopped because of zero gradient. Final point:\n\t"
                    << score << "\t" << pos.x << "\t" << pos.y << std::endl;
        }
        return true;
      }
      auto vn = v.RotateHalfPi();
      vn.Normalize();
      vn *= musician_collision_radius;

      double max_m = 1e6;
      if (v.dx > 0) max_m = std::min(max_m, (p.stage.p2.x - pos.x) / v.dx);
      if (v.dx < 0) max_m = std::min(max_m, (p.stage.p1.x - pos.x) / v.dx);
      if (v.dy > 0) max_m = std::min(max_m, (p.stage.p2.y - pos.y) / v.dy);
      if (v.dy < 0) max_m = std::min(max_m, (p.stage.p1.y - pos.y) / v.dy);
      v *= max_m;
      D2ClosedSegment sp(pos, pos + v);
      for (auto& m : selected) {
        bool adj_required = false;
        if (SquaredDistanceL2(pos + v, m) < dlock) {
          adj_required = true;
        } else {
          D2ClosedSegment sm(m - vn, m + vn);
          adj_required = Intersect(sp, sm);
        }
        if (adj_required) {
          double a = v.LengthSquared();
          double b = v.dx * (pos.x - m.x) + v.dy * (pos.y - m.y);
          double c = SquaredDistanceL2(pos, m) - dlock - 1e-6;
          double d = b * b - a * c;
          if (d < -1e-6)
            std::cout << "Unexpected low D. Please check me." << std::endl;
          d = std::max(d, 0.);
          double ma = (-b - sqrt(d)) / a;
          if (ma > 1.0 + 1e-6) {
            std::cout << "Unexpected high M. Please check me." << std::endl;
            std::cout << "\tP = " << pos.x << "\t" << pos.y << std::endl;
            std::cout << "\tV = " << v.dx << "\t" << v.dy << std::endl;
            std::cout << "\tC = " << m.x << "\t" << m.y << std::endl;
            std::cout << "\t" << a << "\t" << b << "\t" << c << "\t" << d
                      << "\t" << ma << std::endl;
          }
          ma = std::max(0., std::min(ma, 1.0));
          v *= ma;
          sp = D2ClosedSegment(pos, pos + v);
          if (v.LengthSquared() < 1e-12) break;
        }
      }
      if (v.Length() < 1e-6) {
        UpdateBest();
        if (print_log) {
          std::cout << "Iteration " << iteration
                    << " stopped because of blocked gradient. Final point:\n\t"
                    << score << "\t" << pos.x << "\t" << pos.y << std::endl;
        }
        return true;
      }
      auto f = [&](double x) {
        return -Evaluator::DScoreIgnoreBlockedMusician(p, instrument,
                                                       pos + v * x);
      };
      auto x0 = opt::Minimum(f, 0, 1.0, 1e-6);
      pos += v * x0;
      auto new_score =
          Evaluator::DScoreIgnoreBlockedMusician(p, instrument, pos);
      // if (print_log)
      //   std::cout << "\t" << score << " -> " << new_score << std::endl;
      if (new_score <= score + 1e-6) {
        if (print_log) {
          std::cout << "Iteration " << iteration
                    << " stopped because of no improvement. Final point:\n\t"
                    << new_score << "\t" << pos.x << "\t" << pos.y << std::endl;
        }
        score = new_score;
        UpdateBest();
        return true;
      }
      score = new_score;
    }
  }
};
