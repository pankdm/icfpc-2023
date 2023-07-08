#pragma once

#include "base/evaluator.h"
#include "base/problem.h"
#include "base/solution.h"

#include "common/solvers/solver.h"

using BaseSolver = solvers::Solver<Problem, Solution, Evaluator>;
