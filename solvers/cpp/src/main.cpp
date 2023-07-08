#include "base/constants.h"
#include "solvers/base.h"
#include "solvers/greedy1.h"
#include "solvers/greedy2.h"

#include "common/files/command_line.h"
#include "common/solvers/ext/run_n.h"

#include <memory>

void InitCommaneLine(files::CommandLine& cmd) {
  cmd.AddArg("mode", "eval");
  cmd.AddArg("solution", "best");
  cmd.AddArg("solver", "greedy1");
  cmd.AddArg("timelimit", 125);
  cmd.AddArg("nthreads", 4);
  cmd.AddArg("first_problem", 1);
  cmd.AddArg("last_problem", last_problem);
}

BaseSolver::PSolver CreateSolver(const files::CommandLine& cmd,
                                 const std::string& solver_name) {
  auto timelimit = cmd.GetInt("timelimit");
  if (solver_name == "greedy1") {
    return std::make_shared<Greedy1>(timelimit);
  } else if (solver_name == "greedy2") {
    return std::make_shared<Greedy2>(timelimit);
  } else {
    std::cerr << "Unknown solver type: " << solver_name << std::endl;
    exit(-1);
  }
}

int main(int argc, char** argv) {
  files::CommandLine cmd;
  InitCommaneLine(cmd);
  cmd.Parse(argc, argv);

  const auto mode = cmd.GetString("mode");
  if (mode == "eval") {
    // EvaluateSolution(cmd.GetString("solution"));
  } else if (mode == "adjust") {
    // CheckWithAdjuster<adj::Simple>(cmd.GetString("solution"));
  } else if (mode == "update") {
    // UpdateBest(cmd.GetString("solution"));
  } else if (mode == "run") {
    auto solver_name = cmd.GetString("solver");
    auto s = CreateSolver(cmd, solver_name);
    int nthreads = cmd.GetInt("nthreads");
    if (nthreads <= 0)
      solvers::ext::RunN<BaseSolver>(*s, cmd.GetInt("first_problem"),
                                     cmd.GetInt("last_problem"));
    else
      solvers::ext::RunNMT<BaseSolver>(*s, cmd.GetInt("first_problem"),
                                       cmd.GetInt("last_problem"), nthreads);
  } else {
    std::cerr << "Unknown mode " << mode << std::endl;
  }

  return 0;
}