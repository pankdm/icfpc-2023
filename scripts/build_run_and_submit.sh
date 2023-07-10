#!/bin/bash
set -ex

cd ../solvers/cpp
make
# #executing args command
$@
# ./main.solver -mode $MODE -solver $SOLVER  -nthreads $NTHREADS -first_problem $FIRST_PROBLEM -last_problem $LAST_PROBLEM -timelimit $TIMELIMIT
./main.solver -mode update -solution vintlucky
git add ../..
git commit -m 'new solutions'
git fetch
git rebase origin/main
git push
