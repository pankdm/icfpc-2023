#pragma once

#include "base/constants.h"
#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include <iostream>
#include <string>

// Check solution from cache with adjuster.
template <class TAdjuster>
void CheckWithAdjuster(const std::string& solver_name) {
  TAdjuster adj;
  for (unsigned i = 1; i <= last_problem; ++i) {
    Problem p;
    if (!p.Load(std::to_string(i))) continue;
    Solution s;
    if (!s.Load(std::to_string(i), solver_name)) continue;
    Solution snew = s;
    if (adj.Check(p, snew)) {
      auto score_old = Evaluator::Apply(p, s).score;
      auto score_new = Evaluator::Apply(p, snew).score;
      std::cout << "New solution from adjuster for problem " << i << ":\t"
                << score_old << " -> " << score_new << std::endl;
      if (score_new > score_old) {
        snew.Save(solver_name);
      } else {
        assert(false);
      }
    }
  }
}
