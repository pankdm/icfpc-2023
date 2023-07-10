    

from dotenv import load_dotenv
from server.server import get_scoreboard, get_userboard
import os
import json
from types import SimpleNamespace
from slack_integration import post_to_slack

from terminaltables import AsciiTable

import requests
import json
import os


import time

def get_new_score():
    r = requests.get('http://localhost:8000/userboard')
    data = json.loads(r.text)
    s = 0
    for (k, v) in data.items():
        s += int(v)
    return s

path = "./leaderboard.txt"

def get_old_score():
    if os.path.exists(path):
        with open(path, "r") as f:
            return int(float(f.read()))
    return None

def write_new_score(score):
    if not score is None:
        with open(path, "w") as f:
            f.write(str(score) + "\n")

def send_message_to_slack(old_score, new_score):
    if old_score is None: old_score = 0
    if new_score is None: new_score = 0
    if old_score == 0 or new_score == 0:
        return

    msg = "\n".join([
        f'Score change detected: `{int(old_score):,}` -> `{int(new_score):,}`, delta = `{int(new_score-old_score):,}`',
    ])

    print(msg)

    if new_score != old_score:
        from slack_integration import post_to_slack
        post_to_slack(msg)

load_dotenv()
if __name__ == "__main__":
    load_dotenv()
    print (os.environ)
    new_score = get_new_score()
    old_score = get_old_score()
    print("")
    print (old_score, " --> ", new_score)
    send_message_to_slack(old_score, new_score)
    write_new_score(new_score)
