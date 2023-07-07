import json
from dataclasses import dataclass


def meaning_of_life():
    return 42

BORDER_RADIUS = 10

def load_problem(i):
    path = "./problems/{}.json".format(i)
    with open(path, "r") as f:
        data = f.read()
        js_data = json.loads(data)
        return js_data

class Problem:
    def __init__(self, id):
        spec = load_problem(id)
        self.attendees = [(a["x"], a["y"]) for a in spec["attendees"]]
        self.ntastes = len(spec["attendees"][0]["tastes"])
        self.nmus = len(spec["musicians"])
        self.mus_tastes = spec["musicians"]
        self.att_tastes: list[list] = [a["tastes"] for a in spec["attendees"]]
        self.stage_bottom_left = (spec["stage_bottom_left"][0], spec["stage_bottom_left"][1])
        self.stage_size = (spec["stage_width"], spec["stage_height"])
        self.stage_rect = ((spec["stage_bottom_left"][0], spec["stage_bottom_left"][1]), (spec["stage_bottom_left"][0]+spec["stage_width"], spec["stage_bottom_left"][1]+spec["stage_height"]))
        self.stage_min_x = spec["stage_bottom_left"][0]
        self.stage_max_x = spec["stage_bottom_left"][0]+spec["stage_width"]
        self.stage_min_y = spec["stage_bottom_left"][1]
        self.stage_max_y = spec["stage_bottom_left"][1]+spec["stage_height"]
        self.pillars = spec["pillars"]


def load_solution(i, user=""):
    """Solution as a list of (x, y) tuples"""
    path = "../../solutions/{}/{}.json".format(user if user else "_best", i)
    try:
        with open(path, "r") as f:
            data = f.read()
            js_data = json.loads(data)
            return [(p["x"], p["y"]) for p in js_data["placements"]]
    except:  # file doesn't exist or not a valid json
        return None
    
