import json
import itertools
import os
from types import SimpleNamespace
from scripts.problem_ids import get_problem_ids

def get_estimated_scores():
    path = "./metadata/estimated.json"
    if not os.path.exists(path):
        return {}
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data)
        # convert keys to strings
        return {int(i): value for (i, value) in js.items()}

def get_from_file(solution):
    path = f"./metadata/scores/{solution}.json"
    if not os.path.exists(path):
        return {}
    with open(path, "r") as f:
        data = f.read()
        js = json.loads(data)
        # convert keys to strings
        return {int(i): value for (i, value) in js.items()}


def get_our_best_scores():
    maps = {}
    for solution in ["loks_best", "dm_border2", "loks_rgreedy1", "loks_border1", "loks_rgreedy2"]:
        maps[solution] = get_from_file(solution)
    output = {}
    for i in range(1, 91):
        entry = {}
        for solution, data in maps.items():
            entry[solution] = data.get(i, -1)
        output[i] = entry
    return output



def get_problem_stats():
    all_problem_stats = {}
    for i in get_problem_ids():
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
                "room_width": int(js.room_width),
                "room_height": int(js.room_height),
                "stage_width": int(js.stage_width),
                "stage_height": int(js.stage_height),
                "stage_bottom_left": js.stage_bottom_left,
                "pillars": len(js.pillars),
            }
    return all_problem_stats


if __name__ == "__main__":
    problem_stats = get_problem_stats()

    print("problem musicians attendees tastes [w x h]")
    for id, stats in problem_stats.items():
        print(
            "{:2} -> {:5} {:5} {:5} [{} x {}] ".format(
                id,
                stats["musicians"],
                stats["attendees"],
                stats["instruments"],
                stats["stage_width"],
                stats["stage_height"],
            )
        )
