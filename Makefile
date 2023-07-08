PYTHON              := python3
PIP                 := pip3
FLASK               := ${PYTHON} -m flask
EXPORT_PYTHONPATH   := export PYTHONPATH="$(shell pwd)";

install:
	bash ./install.sh

start-server:
	${EXPORT_PYTHONPATH} ${FLASK} --app server/server --debug run --port=8000

start-ui:
	cd ui; pnpm dev

compile-ui:
	cd ui; pnpm build

start-ui-compiled:
	cd ui; pnpm preview
