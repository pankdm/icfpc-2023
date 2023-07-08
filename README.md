# ICFPC-2023

## Team: Snakes, Monkeys and Two Smoking Lambdas

## Installation

1. Use `make` to install dependencies.
2. Create `.env` file with access creds to ICFPC API

```txt
# example .env
ICFPC_USER_EMAIL="team@email.com"
ICFPC_USER_PASSWORD="password"
```

## Usage

- `make start-server` to start utility server
- `make start-ui` to start the visuzliser.

To check server authentication with ICFPC API, do this:

```bash
curl -X POST localhost:8000/check-auth
```

Download all problems:

```bash
curl localhost:8000/problems/download
```

Submit a solution:

```bash
curl -X POST localhost:8000/solutions/<username>/<task_id>
```

Submit all solutions:

```bash
curl -X POST localhost:8000/solutions/username
```

Show problems stats:

```bash
python3 ./scripts/problem_stats.py
```

## Repo Structure

- `./problems`  - problem descriptions
- `./solutions`  - all solutions
  - `./solutions/_base`  - best solutions
  - `./solutions/$USERNAME`  - personal sandboxes
- `./solvers`  - all solvers
  - `./solvers/cpp`  - C++
  - `./solvers/cpp/*.solver`  - binaries for C++ solvers
  - `./solvers/python`  - Python
- `./solvers/python/*.py`  -- python sources корневые скрипты питонячих солверов
- `./server`  -- python flask server. Uses ICFPC API: download leaderboard, upload solutions, provides files for UI
- `./ui`  -- DO NOT GO HERE!! It's a hell of UI! (каша, мёд, говно и пчёлы)
