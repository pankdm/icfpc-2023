#pragma once

#include "base/attendee.h"
#include "base/constants.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/geometry/d2/base.h"
#include "common/geometry/d2/circle.h"
#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/segment.h"
#include "common/geometry/d2/utils/intersect_segment.h"
#include "common/solvers/evaluator.h"

#include <vector>

class Evaluator : public solvers::Evaluator {
 public:
  using Result = solvers::Evaluator::Result;

  static bool Compare(const Result& l, const Result& r) {
    return l.correct ? r.correct ? l.score > r.score : true : false;
  }

  static bool Valid(const Problem& p, const Solution& s) {
    if (p.instruments.size() != s.positions.size()) return false;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      auto& pi = s.positions[i];
      if (!p.stage.Inside(pi)) return false;
      for (unsigned j = 0; j < i; ++j) {
        if (SquaredDistanceL2(pi, s.positions[j]) <
            musician_collision_radius * musician_collision_radius)
          return false;
      }
    }
    return true;
  }

  static bool BlockedByCircle(const D2OpenSegment& segment,
                              const D2Vector& normal, const D2Point& center,
                              double radius) {
    D2OpenSegment s2(center - normal * radius, center + normal * radius);
    return Intersect(segment, s2);
  }

  static bool BlockedByMusucian(const D2OpenSegment& segment,
                                const D2Vector& normal,
                                const D2Point& position) {
    return BlockedByCircle(segment, normal, position, musician_block_radius);
  }

  static bool BlockedByMusucians(const D2OpenSegment& segment,
                                 const D2Vector& normal, unsigned exclude,
                                 const std::vector<D2Point>& positions) {
    for (unsigned i = 0; i < positions.size(); ++i) {
      if (i == exclude) continue;
      if (BlockedByMusucian(segment, normal, positions[i])) return true;
    }
    return false;
  }

  static bool BlockedByPillar(const D2OpenSegment& segment,
                              const D2Vector& normal, const D2Circle& pillar) {
    return BlockedByCircle(segment, normal, pillar.c, pillar.r);
  }

  static bool BlockedByPillars(const D2OpenSegment& segment,
                               const D2Vector& normal,
                               const std::vector<D2Circle>& pillars) {
    for (auto& p : pillars) {
      if (BlockedByPillar(segment, normal, p)) return true;
    }
    return false;
  }

  static bool BlockedByPillar(const Attendee& a, const D2Circle& pillar,
                              const D2Point& position) {
    const auto& pa = a.position;
    const auto& pk = position;
    D2LinePV l(pa, pk);
    l.Normalize();
    auto ln = l.Normal() * pillar.r;
    D2ClosedSegment sak(pa, pk);
    auto& pi = pillar.c;
    D2OpenSegment st(pi - ln, pi + ln);
    if (Intersect(sak, st)) return true;
    return false;
  }

  static bool Blocked(const D2OpenSegment& segment, const D2Vector& normal,
                      const Problem& p, unsigned exclude, const Solution& s) {
    return BlockedByMusucians(segment, normal, exclude, s.positions) ||
           BlockedByPillars(segment, normal, p.pillars);
  }

  static bool Blocked(const Attendee& a, unsigned musician, const Problem& p,
                      const Solution& s) {
    auto &pa = a.position, &pm = s.positions[musician];
    D2OpenSegment sam(pa, pm);
    auto vn = (pm - pa).RotateHalfPi();
    vn.Normalize();
    return Blocked(sam, vn, p, musician, s);
  }

  // DScore for unblocked pair
  static double DScore1(const Attendee& a, unsigned instrument,
                        const D2Point& musician) {
    return score_mult * a.tastes[instrument] /
           SquaredDistanceL2(a.position, musician);
  }

  static int64_t IScore1(const Attendee& a, unsigned instrument,
                         const D2Point& musician) {
    return ceil(DScore1(a, instrument, musician));
  }

  // DScore for Attendee
  static double DScoreIgnoreBlockedAttendee(const Attendee& a, const Problem& p,
                                            const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i)
      dscore += DScore1(a, p.instruments[i], s.positions[i]);
    return dscore;
  }

  // DScore for musician
  static double DScoreIgnoreBlockedMusician(const Problem& p,
                                            unsigned instrument,
                                            const D2Point& musician) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScore1(a, instrument, musician);
    return dscore;
  }

  // DScore for musician
  static double DScoreIgnoreBlockedMusician(const Problem& p, unsigned musician,
                                            const Solution& s) {
    return DScoreIgnoreBlockedMusician(p, p.instruments[musician],
                                       s.positions[musician]);
  }

  static double DScoreIgnoreBlocked(const Problem& p, const Solution& s) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScoreIgnoreBlockedAttendee(a, p, s);
    return dscore;
  }

  // DScore for Attendee
  static double DScoreAttendee(const Attendee& a, const Problem& p,
                               const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, i, p, s)) continue;
      dscore += DScore1(a, p.instruments[i], s.positions[i]);
    }
    return dscore;
  }

  // IScore for Attendee
  static int64_t IScoreAttendee(const Attendee& a, const Problem& p,
                                const Solution& s) {
    int64_t iscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, i, p, s)) continue;
      iscore += IScore1(a, p.instruments[i], s.positions[i]);
    }
    return iscore;
  }

  static double DScore(const Problem& p, const Solution& s) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScoreAttendee(a, p, s);
    return dscore;
  }

  static int64_t IScore(const Problem& p, const Solution& s) {
    int64_t iscore = 0.;
    for (auto& a : p.attendees) iscore += IScoreAttendee(a, p, s);
    return iscore;
  }

  static Result Apply(const Problem& p, const Solution& s) {
    return Valid(p, s) ? Result(true, IScore(p, s)) : Result(false, 0);
  }
};
