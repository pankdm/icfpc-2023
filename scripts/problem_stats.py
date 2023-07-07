import json
import itertools
import os
from types import SimpleNamespace

print("problem musicians attendees tastes [w x h]")

def get_problem_stats():
    all_problem_stats = {}
    for i in itertools.count(start=1):
        path = f"./problems/{i}.json"
        if not os.path.exists(path):
            break
        with open(path, "r") as f:
            data = f.read()
            js = json.loads(data, object_hook=lambda d: SimpleNamespace(**d))
            tastes_musicians = len(set(js.musicians))
            tastes_attendees = set(len(x.tastes) for x in js.attendees)
            assert len(tastes_attendees) == 1
            assert tastes_musicians == next(iter(tastes_attendees))
            all_problem_stats[i] = problem_stats = {
                "musicians": len(js.musicians),
                "attendees": len(js.attendees),
                "instruments": tastes_musicians,
                "stage_width": int(js.stage_width),
                "stage_height": int(js.stage_height),
            }
    return all_problem_stats

if __name__ == '__main__':
    problem_stats = get_problem_stats()
    for id, stats in problem_stats.items():
        print(
            "{:2} -> {:5} {:5} {:5} [{} x {}] ".format(
                id,
                stats["total_musicians"],
                stats["total_attendees"],
                stats["total_instruments"],
                stats["stage_width"],
                stats["stage_height"],
            )
        )
