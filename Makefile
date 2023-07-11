PYTHON              := python3
PIP                 := pip3
FLASK               := ${PYTHON} -m flask
EXPORT_PYTHONPATH   := export PYTHONPATH="$(shell pwd)";

install:
	bash ./install.sh all

install-prompt:
	bash ./install.sh

start-docker:
	docker compose up

rebuild-docker:
	docker compose build

start-server:
	${EXPORT_PYTHONPATH} ${FLASK} --app server/server --debug run --host=0.0.0.0 --port=8000

python-repl:
	PYTHONPATH=. python -i repl.py

start-ui:
	cd ui; pnpm dev

start-ui-server:
	cd ui; pnpm dev:server

compile-ui:
	cd ui; pnpm build

preview-ui:
	cd ui; pnpm preview
