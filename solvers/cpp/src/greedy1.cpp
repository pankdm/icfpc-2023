#include "solvers/greedy1.h"

#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"
#include "utils/one_musician.h"

#include "common/geometry/d2/utils/midpoint.h"

#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./solver PROBLEM_ID" << std::endl;
    return 1;
  }
  Problem p;
  bool b = p.Load(argv[1]);
  if (!b) {
    std::cout << "Problem was not loaded" << std::endl;
    return 2;
  }

  Greedy1 slvr;
  auto s = slvr.Solve(p);

  return 0;
}
