#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage: ./solver PROBLEM_ID" << std::endl;
    return 1;
  }
  Problem p;
  bool b = p.Load(argv[1]);
  std::cout << "Load results: " << b << std::endl;
  return 0;
}
