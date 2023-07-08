SOLVER := main

define USAGE
usage:
  - make -- see this message
  - make SOLVER=solver_name -- compile given solver
endef
export USAGE

compile:
	g++ -std=c++17 -I "./" -O3 -Wall -DNDEBUG ./src/${SOLVER}.cpp -o ./${SOLVER}.solver

compile-fast:
	g++ -std=c++17 -I "./" -O0 -Wall -DNDEBUG ./src/main.cpp -o ./main.solver


usage:
	@echo "$$USAGE"

install-macos:
	which g++ || brew install gcc g++ cmake
