#pragma once

#include "base/evaluator.h"
#include "base/problem.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/point.h"
#include "common/geometry/d2/segment.h"
#include "common/geometry/d2/vector.h"

#include <vector>

class OneMusician2 {
 public:
  D2Point pos;
  unsigned instrument;
  unsigned sindex;
  std::vector<unsigned> audience;
  double score_without_boost;
  double boost_mult;
  double delta_score_without_boost;

 public:
  static std::vector<unsigned> FindAudience(
      D2Point& pos, const Problem& p,
      const std::vector<OneMusician2>& selected) {
    std::vector<unsigned> v;
    for (unsigned i = 0; i < p.attendees.size(); ++i) {
      auto& a = p.attendees[i];
      bool blocked = false;
      D2OpenSegment s(pos, a.position);
      auto sn = (a.position - pos).RotateHalfPi();
      sn.Normalize();
      blocked = blocked || Evaluator::BlockedByPillars(s, sn, p.pillars);
      for (auto& m : selected)
        blocked = blocked || Evaluator::BlockedByMusucian(s, sn, m.pos);
      if (!blocked) v.push_back(i);
    }
    return v;
  }

  static std::vector<unsigned> FindBlockedAudience(
      const std::vector<unsigned>& audience, const D2Point& musician,
      const D2Point& new_musician, const Problem& p) {
    std::vector<unsigned> v;
    for (unsigned i : audience) {
      auto& a = p.attendees[i];
      D2OpenSegment s(musician, a.position);
      auto sn = (a.position - musician).RotateHalfPi();
      sn.Normalize();
      if (Evaluator::BlockedByMusucian(s, sn, new_musician)) v.push_back(i);
    }
    return v;
  }
};
