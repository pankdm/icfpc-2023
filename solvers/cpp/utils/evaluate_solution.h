#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/solvers/ext/evaluate.h"

#include <algorithm>
#include <iostream>

inline void EvaluateSolution(const std::string& solver_name) {
  int64_t total = 0;
  for (unsigned i = 1; i <= last_problem; ++i) {
    auto r = solvers::ext::Evaluate<Evaluator, Problem, Solution>(
        std::to_string(i), solver_name);
    if (r.correct) total += std::max(r.score, 0ll);
    std::cout << "Problem " << std::to_string(1000 + i).substr(1) << "\t"
              << r.correct << "\t" << r.score << std::endl;
  }
  std::cout << "Total = " << total << std::endl;
}

inline void EvaluateSolutionToJson(const std::string& solver_name) {
  int64_t total = 0;

  std::string fileName = "../../metadata/scores/" + solver_name + ".json";
  files::JSON json;
  json.SetDictionary();

  for (unsigned i = 1; i <= last_problem; ++i) {
    auto r = solvers::ext::Evaluate<Evaluator, Problem, Solution>(
        std::to_string(i), solver_name);
    if (r.correct) total += std::max(r.score, 0ll);
    std::cout << "Problem " << std::to_string(1000 + i).substr(1) << "\t"
              << r.correct << "\t" << r.score << std::endl;

    auto key = std::to_string(i);
    auto score = files::JSON((double)r.score);
    if (!json.HasKey(key)) {
      json.Add(key, score);
    } else {
      json.GetValue(key) = score;
    }
  }
  std::cout << "Total = " << total << std::endl;

  std::cout << "Saved to file " << fileName << std::endl;
  json.Save(fileName);
}

inline void UpdateBest(const std::string& solver_name) {
  for (unsigned i = 1; i <= last_problem; ++i) {
    auto b = solvers::ext::UpdateBest<Evaluator, Problem, Solution>(
        std::to_string(i), solver_name, "loks_best");
    if (b) {
      std::cout << "Best was updated for problem: " << i << std::endl;
    }
  }
}
