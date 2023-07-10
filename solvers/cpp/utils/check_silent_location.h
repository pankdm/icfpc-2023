#pragma once

#include "common/geometry/d2/angle_float.h"
#include "common/geometry/d2/axis/rectangle.h"
#include "common/geometry/d2/base.h"
#include "common/geometry/d2/circle.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/line_pv.h"
#include "common/geometry/d2/point.h"
#include "common/geometry/d2/segment.h"
#include "common/geometry/d2/utils/intersect_segment.h"
#include "common/geometry/d2/utils/intersect_segment_dcircle.h"

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

    // Init angles
    D2Point cl, ch;
    if (p.x >= stage.p1.x) {
      if (p.y >= stage.p2.y) {
        cl = D2Point(stage.p1.x, stage.p2.y);
        ch = D2Point(stage.p2.x, stage.p2.y);
      } else {
        assert(p.y <= stage.p1.y);
        cl = D2Point(stage.p2.x, stage.p1.y);
        ch = D2Point(stage.p1.x, stage.p1.y);
      }
    } else {
      // p.x < stage.p1.x
      if (p.y >= stage.p2.y) {
        cl = D2Point(stage.p1.x, stage.p1.y);
        ch = D2Point(stage.p2.x, stage.p2.y);
      } else if (p.y >= stage.p1.y) {
        cl = D2Point(stage.p1.x, stage.p1.y);
        ch = D2Point(stage.p1.x, stage.p2.y);
      } else {
        // p.y < stage.p1.y
        cl = D2Point(stage.p2.x, stage.p1.y);
        ch = D2Point(stage.p1.x, stage.p2.y);
      }
    }
    double a0 = D2Angle(cl - p).RAngle();
    double a1 = D2Angle(ch - p).RAngle();
    assert(a0 <= a1);
    if (print_debug_info) {
      std::cout << "Stage view:\t" << a0 << "\t" << a1 << std::endl;
    }
    D2ClosedSegment pcl(p, cl), pch(p, ch), slh(cl, ch);

    std::vector<std::pair<double, double>> vba;
    for (auto& c : pillars) {
      // Check if pillar on other side of stage.
      if (!Intersect(pcl, c) && !Intersect(pch, c)) {
        D2ClosedSegment pcc(p, c.c);
        if (Intersect(slh, pcc)) continue;
        D2LinePV l(p, c.c);
        if (!IntersectSegment(slh, l)) continue;
      }
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
    for (auto& p : vba) {
      if ((p.first < a0) && (p.second > a1)) return true;
    }
    return false;
  }
};
