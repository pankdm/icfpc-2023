#!/bin/bash

while true; do
    echo "Loop iteration"
    # PYTHONPATH=. python3 scripts/check_leaderboard.py
    PYTHONPATH=. python3 scripts/check_userboard.py
    date
    sleep 60  # Sleep for 10 minutes
done
