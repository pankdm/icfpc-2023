
import json
from terminaltables import AsciiTable

latest = "./leaderboard_archive/1688983374.json"

with open(latest, "r") as f:
    data = json.loads(f.read())
    table_data = [ ['Rank', 'Team', 'Score'] ]
    for idx, r in enumerate(data["scoreboard"]):
        table_data.append([idx + 1, r["username"], r["score"]])
    table = AsciiTable(table_data)

    print(table.table)
    with open("./frozen_leaderboard.txt", "wt") as g:
        g.write(table.table)
  

    

