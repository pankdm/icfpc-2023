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
	g++ -std=c++11 ./src/${SOLVER}.cpp -o ./${SOLVER}

usage:
	@echo "$$USAGE"

install-macos:
	which g++ || brew install gcc g++ cmake
