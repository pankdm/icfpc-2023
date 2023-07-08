#pragma once

#include "base/attendee.h"
#include "base/constants.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/line_pv.h"
#include "common/solvers/evaluator.h"

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

  static bool Blocked(const Attendee& a, const Solution& s, unsigned k) {
    auto &pa = a.position, &pk = s.positions[k];
    auto d2 = SquaredDistanceL2(pa, pk);
    D2LinePV l(pa, pk);
    l.Normalize();
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      auto& pi = s.positions[i];
      if ((i != k) && (l(pi) < musician_block_radius) &&
          (SquaredDistanceL2(pa, pi) < d2))
        return true;
    }
    return false;
  }

  // DScore for Attendee
  double DScoreIgnoreBlockedAttendee(const Attendee& a, const Problem& p,
                                     const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      dscore += score_mult * a.tastes[p.instruments[i]] /
                SquaredDistanceL2(a.position, s.positions[i]);
    }
    return dscore;
  }

  // DScore for musician
  double DScoreIgnoreBlockedMusician(const Problem& p, unsigned instrument,
                                     const D2Point& pos) {
    double dscore = 0.;
    for (auto& a : p.attendees)
      dscore += score_mult * a.tastes[instrument] /
                SquaredDistanceL2(a.position, pos);
    return dscore;
  }

  // DScore for musician
  double DScoreIgnoreBlockedMusician(const Problem& p, const Solution& s,
                                     unsigned k) {
    return DScoreIgnoreBlockedMusician(p, p.instruments[k], s.positions[k]);
  }

  double DScoreIgnoreBlocked(const Problem& p, const Solution& s) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScoreIgnoreBlockedAttendee(a, p, s);
    return dscore;
  }

  // DScore for Attendee
  double DScoreAttendee(const Attendee& a, const Problem& p,
                        const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, s, i)) continue;
      dscore += score_mult * a.tastes[p.instruments[i]] /
                SquaredDistanceL2(a.position, s.positions[i]);
    }
    return dscore;
  }

  // IScore for Attendee
  int64_t IScoreAttendee(const Attendee& a, const Problem& p,
                         const Solution& s) {
    int64_t iscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (Blocked(a, s, i)) continue;
      iscore += ceil(score_mult * a.tastes[p.instruments[i]] /
                     SquaredDistanceL2(a.position, s.positions[i]));
    }
    return iscore;
  }

  double DScore(const Problem& p, const Solution& s) {
    double dscore = 0.;
    for (auto& a : p.attendees) dscore += DScoreAttendee(a, p, s);
    return dscore;
  }

  int64_t IScore(const Problem& p, const Solution& s) {
    int64_t iscore = 0.;
    for (auto& a : p.attendees) iscore += IScoreAttendee(a, p, s);
    return iscore;
  }

  Result Apply(const Problem& p, const Solution& s) {
    return Valid(p, s) ? Result(true, IScore(p, s)) : Result(false, 0);
  }
};