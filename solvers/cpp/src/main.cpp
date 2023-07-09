#include "adjusters/assignment.h"
#include "adjusters/max_volume.h"
#include "adjusters/swaps.h"
#include "base/constants.h"
#include "solvers/base.h"
#include "solvers/border.h"
#include "solvers/greedy1.h"
#include "solvers/greedy2.h"
#include "solvers/greedy3.h"
#include "solvers/shaker.h"
#include "utils/check_silent_location.h"
#include "utils/check_with_adjuster.h"
#include "utils/estimate_max_score.h"
#include "utils/evaluate_solution.h"

#include "common/files/command_line.h"
#include "common/solvers/ext/run_n.h"

#include <memory>

void InitCommaneLine(files::CommandLine& cmd) {
  cmd.AddArg("mode", "eval");
  cmd.AddArg("solution", "best");
  cmd.AddArg("solver", "greedy1");
  cmd.AddArg("adjuster", "assignment");
  cmd.AddArg("timelimit", 125);
  cmd.AddArg("extra", 0);
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
    return std::make_shared<Greedy2>(timelimit, cmd.GetInt("extra"));
  } else if (solver_name == "greedy3") {
    return std::make_shared<Greedy3>(timelimit, cmd.GetInt("extra"));
  } else if (solver_name == "shaker") {
    return std::make_shared<Shaker>(timelimit);
  } else if (solver_name == "border") {
    return std::make_shared<BorderSolver>(timelimit);
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
    EvaluateSolution(cmd.GetString("solution"));
  } else if (mode == "adjust") {
    auto name = cmd.GetString("adjuster");
    if (name == "assignment") {
      CheckWithAdjuster<AdjusterAssignment>(cmd.GetString("solution"));
    } else if (name == "max_volume") {
      CheckWithAdjuster<AdjusterMaxVolume>(cmd.GetString("solution"));
    } else if (name == "swaps") {
      CheckWithAdjuster<AdjusterSwaps>(cmd.GetString("solution"));
    } else {
      std::cerr << "Unknown adjuster name " << name << std::endl;
    }
  } else if (mode == "update") {
    UpdateBest(cmd.GetString("solution"));
  } else if (mode == "estimate") {
    EstimateAllProblems();
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
  } else if (mode == "test") {
    // Code for temporary testing
    for (unsigned i = 56; i <= 90; ++i) {
      Problem p;
      p.Load(std::to_string(i));
      Solution s;
      s.Load(std::to_string(i), "loks_best");
      unsigned silent = 0;
      for (auto& a : p.attendees) {
        if (CheckSilentLocation::Check(p.stage, p.pillars, a.position, false)) {
          ++silent;
          auto d = Evaluator::DScoreAttendee(a, p, s);
          if (d != 0.) {
            std::cout << "\t" << a.position.x << "\t" << a.position.y << "\t"
                      << d << std::endl;
            CheckSilentLocation::Check(p.stage, p.pillars, a.position, true);
          }
        }
      }
      std::cout << i << "\t" << silent << "\t" << p.attendees.size() << "\t"
                << p.pillars.size() << std::endl;
    }
  } else {
    std::cerr << "Unknown mode " << mode << std::endl;
  }

  return 0;
}
