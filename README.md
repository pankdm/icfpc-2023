# ICFPC-2023

## Team: Snakes, Monkeys and Two Smoking Lambdas

<img width="1516" alt="image" src="https://github.com/pankdm/icfpc-2023/assets/3831006/3a44b0b7-0419-47b5-89cb-82c630b4d430">

## Installation

1. Create `.env` file with access creds to ICFPC API

```txt
# example .env
ICFPC_USER_EMAIL="team@email.com"
ICFPC_USER_PASSWORD="password"
```

2. If you have Docker and Docker Compose installed - you're done!
3. Otherwise, use `make` to install dependencies.

## Running locally

If you have Docker and Docker Compose:

- `make start-docker` to start everything at once locally,

...otherwise:

- `make start-server` to start utility server
- `make start-ui` to start the visuzliser.

## Usage

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
- `./solvers/python/*.py` -- python sources
- `./server` -- python flask server. Uses ICFPC API: download leaderboard, upload solutions, provides files for UI
- `./ui` -- UI: visualizer, scoreboard, leaderboard
