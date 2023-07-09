#pragma once

#include "common/geometry/d2/angle_float.h"
#include "common/geometry/d2/axis/rectangle.h"
#include "common/geometry/d2/base.h"
#include "common/geometry/d2/circle.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/point.h"
// #include "common/geometry/d2/segment.h"
// #include "common/geometry/d2/utils/intersect_segment.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

class CheckSilentLocation {
 public:
  static bool Check(const D2ARectangle& stage,
                    const std::vector<D2Circle>& pillars, const D2Point& p,
                    bool print_debug_info = false) {
    if (pillars.empty()) return false;
    if (p.x >= stage.p2.x) {
      // Swap to care less about angles
      double xs = stage.p1.x + stage.p2.x;
      auto pillars2 = pillars;
      for (auto& c : pillars2) c.c.x = xs - c.c.x;
      return Check(stage, pillars2, {xs - p.x, p.y}, print_debug_info);
    }
    std::vector<std::pair<double, double>> vba;
    // Check if pillar on other side of stage.
    for (auto& c : pillars) {
      if (((c.c.x - stage.p1.x) * (p.x - stage.p1.x) < 0) ||
          ((c.c.x - stage.p2.x) * (p.x - stage.p2.x) < 0) ||
          ((c.c.y - stage.p1.y) * (p.y - stage.p1.y) < 0) ||
          ((c.c.y - stage.p2.y) * (p.y - stage.p2.y) < 0))
        continue;
      double a = D2Angle(c.c - p).RAngle();
      double da = asin(c.r / DistanceL2(c.c, p));
      vba.push_back({a - da, a + da});
      if (print_debug_info) {
        std::cout << "Add angle\t" << vba.back().first << "\t"
                  << vba.back().second << std::endl;
      }
    }
    std::sort(vba.begin(), vba.end(),
              [](const auto& l, const auto& r) { return l.first < r.first; });
    unsigned new_size = 0;
    for (unsigned i = 1; i < vba.size(); ++i) {
      if (vba[i].first < vba[new_size].second) {
        vba[new_size].second = std::max(vba[new_size].second, vba[i].second);
      } else {
        vba[++new_size] = vba[i];
      }
    }
    vba.resize(new_size + 1);
    if (print_debug_info) {
      std::cout << "After merge:" << std::endl;
      for (auto& pa : vba) {
        std::cout << "\t" << pa.first << "\t" << pa.second << std::endl;
      }
    }
    std::vector<double> va4;
    for (unsigned i = 0; i < 4; ++i) {
      D2Point pi((i < 2) ? stage.p1.x : stage.p2.x,
                 (i & 1) ? stage.p2.y : stage.p1.y);
      va4.push_back(D2Angle(pi - p).RAngle());
    }
    auto a0 = *std::min_element(va4.begin(), va4.end());
    auto a1 = *std::max_element(va4.begin(), va4.end());
    if (print_debug_info) {
      std::cout << "Stage view:" << std::endl;
      for (auto& aa : va4) std::cout << "\t" << aa << std::endl;
      std::cout << "\t" << a0 << "\t" << a1 << std::endl;
    }
    for (auto& p : vba) {
      if ((p.first < a0) && (p.second > a1)) return true;
    }
    return false;
  }
};
