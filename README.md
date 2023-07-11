# ICFPC-2023

## Team: Snakes, Monkeys and Two Smoking Lambdas

## Installation

1. Create `.env` file with access creds to ICFPC API

```txt
# example .env
ICFPC_USER_EMAIL="team@email.com"
ICFPC_USER_PASSWORD="password"
```

2. If you have Docker and Docker Compose installed in the system - use `make start-docker` to run locally.
3. Otherwise, use `make` to install dependencies.

## Usage

If you have Docker and Docker Compose:

- `make start-docker` to start everything at once locally,

...otherwise:

- `make start-server` to start utility server
- `make start-ui` to start the visuzliser.

- To check server authentication with ICFPC API, do this:

```bash
curl -X POST localhost:8000/check-auth
```

- Download all problems:

```bash
curl localhost:8000/problems/download
```

- Submit a solution:

```bash
curl -X POST localhost:8000/solutions/<username>/<task_id>
```

- Submit all solutions:

```bash
curl -X POST localhost:8000/solutions/username
```

- Show problems stats:

```bash
python3 ./scripts/problem_stats.py
```

## Scripts

- Run leaderboard monitoring and post updates to slack:

```bash
bash ./scripts/monitor_leaderboard.sh
```

- Update all metadata scores for solutions:

```bash
zsh ./scripts/update_metadata_scores.sh
```

- Update scores and previews for `loks_best`:

```bash
zsh ./scripts/update_best_meta_and_push.sh
```

- Update solution in `best_loks` depending on local folder:

```bash
make compile && ./main.solver -mode update -solution dm_border2
```

- Submit all solutions from a folder:

```bash
for i in `jot - 1 90`; do bash scripts/submit.sh loks_best $i; done
```

## Repo Structure

- `./problems` - problem descriptions
- `./solutions` - all solutions
  - `./solutions/_base` - best solutions
  - `./solutions/$USERNAME` - personal sandboxes
- `./solvers` - all solvers
  - `./solvers/cpp` - C++
  - `./solvers/cpp/*.solver` - binaries for C++ solvers
  - `./solvers/python` - Python
- `./solvers/python/*.py` -- python sources корневые скрипты питонячих солверов
- `./server` -- python flask server. Uses ICFPC API: download leaderboard, upload solutions, provides files for UI
- `./ui` -- DO NOT GO HERE!! It's a hell of UI! (каша, мёд, говно и пчёлы)
