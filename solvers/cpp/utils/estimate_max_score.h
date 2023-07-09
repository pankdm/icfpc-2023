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

  double total_score = 0;
  for (int ins_id = 0; ins_id < p.total_instruments; ++ins_id) {
    if (ins_id % 100 == 0) {
      std::cout << "instrument = " << ins_id << std::endl;
    }
    std::vector<double> possible_scores;
    for (const auto position : border) {
      int instrument_score = 0;
      for (auto& a : p.attendees) {
        instrument_score += ceil(score_mult * a.tastes[ins_id] /
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
