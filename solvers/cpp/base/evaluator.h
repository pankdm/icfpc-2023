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

#include <string>
#include <vector>

class Evaluator : public solvers::Evaluator {
 public:
  using Result = solvers::Evaluator::Result;

  static bool Compare(const Result& l, const Result& r) {
    return l.correct ? r.correct ? l.score > r.score : true : false;
  }

  // Check if solution valid
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

  // Different functions to check blocks
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

  static bool Blocked(const D2OpenSegment& segment, const D2Vector& normal,
                      const Problem& p,
                      const std::vector<D2Point>& musicians_to_check) {
    return BlockedByMusucians(segment, normal, musicians_to_check.size(),
                              musicians_to_check) ||
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

  static bool Blocked(const Attendee& a, const D2Point& musician,
                      const Problem& p,
                      const std::vector<D2Point>& musicians_to_check) {
    auto &pa = a.position, &pm = musician;
    D2OpenSegment sam(pa, pm);
    auto vn = (pm - pa).RotateHalfPi();
    vn.Normalize();
    return Blocked(sam, vn, p, musicians_to_check);
  }

  // Score boost
  static std::vector<double> ScoreBoost(const Problem& p, const Solution& s) {
    if (p.instruments.size() != s.positions.size()) return {};
    std::vector<double> v(s.positions.size(), 1.0);
    if (p.Lighting()) return v;
    for (unsigned i = 0; i < v.size(); ++i) {
      for (auto j : p.musicians[p.instruments[i]]) {
        if (j == i) continue;
        v[i] += 1.0 / DistanceL2(s.positions[i], s.positions[j]);
      }
    }
    return v;
  }

  // DScore for unblocked pair
  static double DScoreRaw(const Attendee& a, unsigned instrument,
                          const D2Point& musician) {
    return score_mult * a.tastes[instrument] /
           SquaredDistanceL2(a.position, musician);
  }

  static double DScoreRaw(const Attendee& a, unsigned instrument,
                          const D2Point& musician, double volume,
                          double boost) {
    return volume * boost * DScoreRaw(a, instrument, musician);
  }

  static int64_t IScoreRaw(const Attendee& a, unsigned instrument,
                           const D2Point& musician) {
    return ceil(DScoreRaw(a, instrument, musician));
  }

  static int64_t IScoreRaw(const Attendee& a, unsigned instrument,
                           const D2Point& musician, double volume,
                           double boost) {
    return ceil(volume * boost * IScoreRaw(a, instrument, musician));
  }

  // DScore for attendee
  static double DScoreIgnoreBlockedAttendee(const Attendee& a, const Problem& p,
                                            const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i)
      dscore += DScoreRaw(a, p.instruments[i], s.positions[i]);
    return dscore;
  }

  static double DScoreIgnoreBlockedAttendee(const Attendee& a, const Problem& p,
                                            const Solution& s,
                                            const std::vector<double>& vboost) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i)
      dscore += DScoreRaw(a, p.instruments[i], s.positions[i], s.Volume(i),
                          vboost[i]);
    return dscore;
  }

  // DScore for musician
  static double DScoreIgnoreBlockedMusician(const Problem& p,
                                            unsigned instrument,
                                            const D2Point& musician) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScoreRaw(a, instrument, musician);
    return dscore;
  }

  static double DScoreIgnoreBlockedMusician(const Problem& p,
                                            unsigned instrument,
                                            const D2Point& musician,
                                            double volume, double boost) {
    return volume * boost *
           DScoreIgnoreBlockedMusician(p, instrument, musician);
  }

  static double DScoreIgnoreBlockedMusician(const Problem& p, unsigned musician,
                                            const Solution& s) {
    return DScoreIgnoreBlockedMusician(p, p.instruments[musician],
                                       s.positions[musician]);
  }

  static double DScoreIgnoreBlockedMusician(const Problem& p, unsigned musician,
                                            const Solution& s, double boost) {
    return s.Volume(musician) * boost *
           DScoreIgnoreBlockedMusician(p, musician, s);
  }

  // Full DScore ignore blocked
  static double DScoreIgnoreBlockedNoBoost(const Problem& p,
                                           const Solution& s) {
    double dscore = 0.;
    for (unsigned k = 0; k < s.positions.size(); ++k)
      dscore += DScoreIgnoreBlockedMusician(p, k, s);
    return dscore;
  }

  static double DScoreIgnoreBlocked(const Problem& p, const Solution& s,
                                    const std::vector<double>& vboost) {
    double dscore = 0.;
    for (unsigned k = 0; k < s.positions.size(); ++k)
      dscore += DScoreIgnoreBlockedMusician(p, k, s, vboost[k]);
    return dscore;
  }

  static double DScoreIgnoreBlocked(const Problem& p, const Solution& s) {
    return DScoreIgnoreBlocked(p, s, ScoreBoost(p, s));
  }

  // DScore for Attendee
  static double DScoreAttendee(const Attendee& a, const Problem& p,
                               const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, i, p, s)) continue;
      dscore += DScoreRaw(a, p.instruments[i], s.positions[i]);
    }
    return dscore;
  }

  static double DScoreAttendee(const Attendee& a, const Problem& p,
                               const Solution& s,
                               const std::vector<double>& vboost) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, i, p, s)) continue;
      dscore += DScoreRaw(a, p.instruments[i], s.positions[i], s.Volume(i),
                          vboost[i]);
    }
    return dscore;
  }

  // IScore for Attendee
  static int64_t IScoreAttendee(const Attendee& a, const Problem& p,
                                const Solution& s,
                                const std::vector<double>& vboost) {
    int64_t iscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, i, p, s)) continue;
      iscore += IScoreRaw(a, p.instruments[i], s.positions[i], s.Volume(i),
                          vboost[i]);
    }
    return iscore;
  }

  // DScore for musician
  static double DScoreMusician(const Problem& p, unsigned musician,
                               const Solution& s) {
    double dscore = 0.;
    for (auto& a : p.attendees) {
      if (Blocked(a, musician, p, s)) continue;
      dscore += DScoreRaw(a, p.instruments[musician], s.positions[musician]);
    }
    return dscore;
  }

  static double DScoreMusician(const Problem& p, unsigned musician,
                               const Solution& s, double boost) {
    return s.Volume(musician) * boost * DScoreMusician(p, musician, s);
  }

  static double DScoreMusician(const Problem& p, unsigned instrument,
                               const D2Point& musician,
                               const std::vector<D2Point>& musicians_to_check) {
    double dscore = 0.;
    for (auto& a : p.attendees) {
      if (Blocked(a, musician, p, musicians_to_check)) continue;
      dscore += DScoreRaw(a, instrument, musician);
    }
    return dscore;
  }

  // IScore for musician
  static int64_t IScoreMusician(const Problem& p, unsigned musician,
                                const Solution& s, double boost) {
    int64_t iscore = 0.;
    for (auto& a : p.attendees) {
      if (Blocked(a, musician, p, s)) continue;
      iscore += IScoreRaw(a, p.instruments[musician], s.positions[musician],
                          s.Volume(musician), boost);
    }
    return iscore;
  }

  // Full DScore
  static double DScoreNoBoost(const Problem& p, const Solution& s) {
    double dscore = 0.;
    for (unsigned k = 0; k < s.positions.size(); ++k)
      dscore += DScoreMusician(p, k, s);
    return dscore;
  }

  static double DScore(const Problem& p, const Solution& s,
                       const std::vector<double>& vboost) {
    double dscore = 0.;
    for (unsigned k = 0; k < s.positions.size(); ++k)
      dscore += DScoreMusician(p, k, s, vboost[k]);
    return dscore;
  }

  static double DScore(const Problem& p, const Solution& s) {
    return DScore(p, s, ScoreBoost(p, s));
  }

  // Full IScore
  static int64_t IScore(const Problem& p, const Solution& s,
                        const std::vector<double>& vboost) {
    int64_t iscore = 0.;
    for (unsigned k = 0; k < s.positions.size(); ++k)
      iscore += IScoreMusician(p, k, s, vboost[k]);
    return iscore;
  }

  static int64_t IScore(const Problem& p, const Solution& s) {
    return IScore(p, s, ScoreBoost(p, s));
  }

  static Result Apply(const Problem& p, const Solution& s) {
    return Valid(p, s) ? Result(true, IScore(p, s)) : Result(false, 0);
  }
};
