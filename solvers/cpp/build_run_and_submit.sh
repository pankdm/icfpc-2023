#!/bin/bash
set -ex
# m./b  @
./main.solver -mode update -solution vintlucky
git add .
git commit -m 'new solutions'
git fetch
git rebase origin/main
git push
