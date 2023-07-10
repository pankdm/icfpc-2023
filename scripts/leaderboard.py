

from server.server import get_scoreboard
from dotenv import load_dotenv
import os
import json
from types import SimpleNamespace
from slack_integration import post_to_slack

from terminaltables import AsciiTable

import time


path = "./leaderboard.txt"

def get_new_score():
    data = get_scoreboard()
    # print (data)
    team = os.environ['ICFPC_USER_EMAIL']
    for entry in data['scoreboard']:
        if entry['username'] == team:
            print (entry)
            return (entry['score'], data)
    return (None, data)

def get_old_score():
    if os.path.exists(path):
        with open(path, "r") as f:
            return float(f.read())
    return None



def write_new_score(score):
    if not score is None:
        with open(path, "w") as f:
            f.write(str(score) + "\n")

def dump_json(data):
    current_timestamp = time.time()
    ts = int(current_timestamp)
    archive = f"leaderboard_archive/{ts}.json"
    print(f'dumping to {archive}')
    
    with open(archive, "w") as f:
        f.write(json.dumps(data, indent=2))

def send_message_to_slack(old_score, new_score, data):
    if old_score is None: old_score = 0
    if new_score is None: new_score = 0
    if old_score == 0 or new_score == 0:
        return
    # header
    table_data = [ ['Rank', 'Team', 'Score'] ]


    rows = []
    sorted_data = list(sorted(data['scoreboard'], key = lambda x: x['score'], reverse=True))
    data['scoreboard'] = sorted_data
    our_idx = 0
    for (idx, entry) in enumerate(data['scoreboard']):
        team = os.environ['ICFPC_USER_EMAIL']
        if entry['username'] == team:
            prefix = ">> "
            our_idx = idx
        else:
            prefix = "   "
        
        str_idx = prefix + str(idx+1)
        rows.append([str_idx, entry['username'], f"{int(entry['score']):,}"])

    filtered_rows = rows[our_idx - 7: our_idx + 4]
    for r in filtered_rows:
        table_data.append(r)

    # def add_table(i, our):
    #     prefix = '>> ' if our else '   '
    #     str_i = prefix + str(i + 1)
    #     now = self.ranking[i]
    #     table_data.append(now.to_table(str_i))

    # for i in range(start_index, index + 2):
    #     add_table(i, i == index)

    table = AsciiTable(table_data)

    msg = "\n".join([
        f'Score change detected: `{int(old_score):,}` -> `{int(new_score):,}`, delta = `{int(new_score-old_score):,}`',
        '```',
        table.table,
        '```'
    ])

    print(msg)

    if new_score != old_score:
        dump_json(data)
        post_to_slack(msg)

if __name__ == "__main__":
    load_dotenv()
    # print(os.environ)
    (new_score, data) = get_new_score()
    old_score = get_old_score()
    print (old_score)
    send_message_to_slack(old_score, new_score, data)
    write_new_score(new_score)

