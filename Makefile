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

preview-ui:
	cd ui; pnpm preview

deploy-ui:
	cd ui; pnpm build && surge dist icfpc2023-snakes-lambdas.surge.sh
