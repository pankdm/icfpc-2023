define USAGE
usage:
  - make -- see this message
  - make SOLVER=solver_name -- compile given solver
endef
export USAGE

compile:
ifndef SOLVER
	make usage
	exit 1
endif
	g++ -std=c++11 -I "./" ./src/${SOLVER}.cpp -o ./${SOLVER}.solver

usage:
	@echo "$$USAGE"

install-macos:
	which g++ || brew install gcc g++ cmake
