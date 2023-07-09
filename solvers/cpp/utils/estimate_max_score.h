#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "solvers/base.h"
#include "utils/one_musician.h"

#include "common/geometry/d2/distance/distance_l2.h"
#include "common/geometry/d2/point.h"
#include "common/solvers/solver.h"
#include "common/timer.h"

#include <algorithm>
#include <utility>
#include <vector>

inline double EstimateProblem(const Problem& p) {
  const double STEP = 5;
  std::vector<int> instrument_count(p.total_instruments);
  for (int i = 0; i < p.instruments.size(); ++i) {
    instrument_count[p.instruments[i]] += 1;
  }

  std::vector<D2Point> border;
  for (double x = p.stage.p1.x; x <= p.stage.p2.x; x += STEP) {
    border.push_back(D2Point{x, p.stage.p1.y});
    border.push_back(D2Point{x, p.stage.p2.y});
  }
  for (double y = p.stage.p1.y; y <= p.stage.p2.y; y += STEP) {
    border.push_back(D2Point{p.stage.p1.x, y});
    border.push_back(D2Point{p.stage.p2.x, y});
  }

  std::vector<std::vector<int>> visible_idx(border.size());
  for (int b_idx = 0; b_idx < border.size(); ++b_idx) {
    const auto& position = border[b_idx];
    for (int i = 0; i < p.attendees.size(); ++i) {
      bool any_blocked = false;
      const auto& a = p.attendees[i];
      for (const auto& pillar : p.pillars) {
        if (Evaluator::BlockedByPillar(a, pillar, position)) {
          any_blocked = true;
          break;
        }
      }
      if (!any_blocked) {
        visible_idx[b_idx].push_back(i);
      }
    }
  }

  double total_score = 0;
  for (int ins_id = 0; ins_id < p.total_instruments; ++ins_id) {
    if (ins_id % 100 == 0) {
      std::cout << "instrument = " << ins_id << std::endl;
    }
    std::vector<double> possible_scores;
    for (int b_idx = 0; b_idx < border.size(); ++b_idx) {
      const auto& position = border[b_idx];
      int instrument_score = 0;
      for (const auto a_idx : visible_idx[b_idx]) {
        const auto& a = p.attendees[a_idx];
        instrument_score += ceil(10 * score_mult * a.tastes[ins_id] /
                                 SquaredDistanceL2(a.position, position));
      }
      possible_scores.push_back(instrument_score);
    }
    sort(std::begin(possible_scores), std::end(possible_scores));
    reverse(std::begin(possible_scores), std::end(possible_scores));
    for (int i = 0; i < instrument_count[ins_id]; ++i) {
      if (i >= possible_scores.size()) {
        break;
      }
      total_score += std::max(possible_scores[i], 0.0);
    }
  }
  return total_score;
}

inline void EstimateAllProblems() {
  std::vector<double> scores(last_problem + 1);
  // for (int i = 1; i <= last_problem; ++i) {
  std::string fileName = "../../metadata/estimated.json";
  files::JSON json;
  if (!json.Load(fileName)) {
    json.SetDictionary();
  }

  for (int i = 1; i <= last_problem; ++i) {
    // for (int i = 88; i <= 88; ++i) {
    Problem p;
    const std::string pid = std::to_string(i);
    if (!p.Load(pid)) continue;
    double score = EstimateProblem(p);
    scores[i] = score;
    std::cout << "Estimated #" << i << " as " << score << std::endl;

    auto key = std::to_string(i);
    if (!json.HasKey(key)) {
      json.Add(key, files::JSON(score));
    } else {
      json.GetValue(key) = files::JSON(score);
    }
    json.Save(fileName);
  }
}
