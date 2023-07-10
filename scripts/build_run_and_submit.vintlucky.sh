#!/bin/bash
set -ex

cd ../solvers/cpp
make
./main.solver -mode run -solver rgreedy2  -nthreads 8 -first_problem 11 -last_problem 30 -timelimit 120
./main.solver -mode update -solution vintlucky
git add ../..
git commit -m 'new solutions'
git fetch
git rebase origin/main
git push

cd ../../scripts
./submit_all.sh loks_rgreedy2
