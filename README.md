# ICFPC-2023
## Team: Snakes, Monkeys and Two Smoking Lambdas

## Installation
1. Use `make` to install dependencies.
2. Create `.env` file with access creds to ICFPC API
```
# example .env
ICFPC_USER_EMAIL="team@email.com"
ICFPC_USER_PASSWORD="password"
```

## Usage
Use `make start-server` to start utility server and `make start-ui` to start the visuzliser.

To check server authentication with ICFPC API, do this:
```
curl -X POST localhost:8000/check-auth
```
