#!/bin/bash

while true; do
    echo "Loop iteration"
    PYTHONPATH=. python3 scripts/leaderboard.py
    date
    sleep 600  # Sleep for 10 minutes
done
