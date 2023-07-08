#pragma once

#include "base/attendee.h"
#include "base/constants.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/line_pv.h"
#include "common/geometry/d2/segment.h"
#include "common/geometry/d2/utils/intersect_segment.h"
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
    D2LinePV l(pa, pk);
    l.Normalize();
    auto ln = l.Normal() * musician_block_radius;
    D2ClosedSegment sak(pa, pk);
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      if (i == k) continue;
      auto& pi = s.positions[i];
      D2ClosedSegment st(pi - ln, pi + ln);
      if (Intersect(sak, st)) return true;
    }
    return false;
  }

  // DScore for Attendee
  static double DScoreIgnoreBlockedAttendee(const Attendee& a, const Problem& p,
                                            const Solution& s) {
    double dscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      dscore += score_mult * a.tastes[p.instruments[i]] /
                SquaredDistanceL2(a.position, s.positions[i]);
    }
    return dscore;
  }

  // DScore for musician
  static double DScoreIgnoreBlockedMusician(const Problem& p,
                                            unsigned instrument,
                                            const D2Point& pos) {
    double dscore = 0.;
    for (auto& a : p.attendees)
      dscore += score_mult * a.tastes[instrument] /
                SquaredDistanceL2(a.position, pos);
    return dscore;
  }

  // DScore for musician
  static double DScoreIgnoreBlockedMusician(const Problem& p, const Solution& s,
                                            unsigned k) {
    return DScoreIgnoreBlockedMusician(p, p.instruments[k], s.positions[k]);
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
      if (Blocked(a, s, i)) continue;
      dscore += score_mult * a.tastes[p.instruments[i]] /
                SquaredDistanceL2(a.position, s.positions[i]);
    }
    return dscore;
  }

  // IScore for Attendee
  static int64_t IScoreAttendee(const Attendee& a, const Problem& p,
                                const Solution& s) {
    int64_t iscore = 0.;
    for (unsigned i = 0; i < s.positions.size(); ++i) {
      // if (Blocked(a, s, i)) {
      //   std::cout << "Blocked score = "
      //             << ceil(score_mult * a.tastes[p.instruments[i]] /
      //                     SquaredDistanceL2(a.position, s.positions[i]))
      //             << std::endl;
      // }
      if (Blocked(a, s, i)) continue;
      iscore += ceil(score_mult * a.tastes[p.instruments[i]] /
                     SquaredDistanceL2(a.position, s.positions[i]));
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
