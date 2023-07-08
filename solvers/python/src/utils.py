import json
from dataclasses import dataclass
import math


def meaning_of_life():
    return 42

BORDER_RADIUS = 10
NEARBY_MUSICIAN_RADIUS = 10
BORDER_RADIUS_SQUARE = 100
BLOCKING_RADIUS = 5


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
        self.mus_tastes: list[int] = spec["musicians"]
        self.att_tastes: list[list[int]] = [a["tastes"] for a in spec["attendees"]]
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
    

def single_score(x, y, px, py, taste):
    d2 = (x - px) ** 2 + (y - py) ** 2
    return math.ceil(10**6 * taste / d2)

def dist_pt_line(pt: tuple, line_start: tuple, line_end: tuple):
    # length of perpendicular line from point pt to line going through line_start and line_end
    x0, y0 = pt
    x1, y1 = line_start
    x2, y2 = line_end
    triangle_area2 = abs(x0*(y1-y2) + x1*(y2-y0) + x2*(y0-y1))
    return triangle_area2 / math.sqrt((y2-y1)**2 + (x2-x1)**2)


def solution_score(problem_id: int, solution: list[tuple]):
    pr = Problem(problem_id)
    if not len(solution) == pr.nmus:
        print("Wrong number of musicians")
        return 0
    nmus = len(solution)
    for i in range(nmus):
        for j in range(i+1, nmus):
            if (solution[i][0]-solution[j][0])**2 + (solution[i][1]-solution[j][1])**2 < BORDER_RADIUS_SQUARE:
                print("Some musicians are too close to each other")
                return 0
    score = 0
    for m, (x, y) in enumerate(solution):
        if not (pr.stage_min_x+BORDER_RADIUS <= x <= pr.stage_max_x-BORDER_RADIUS and pr.stage_min_y+BORDER_RADIUS <= y <= pr.stage_max_y-BORDER_RADIUS):
            print("Some musicians are outside the stage")
            return 0
        for i, (px, py) in enumerate(pr.attendees):
            if any(dist_pt_line((x2,y2), (x,y), (px,py))<=BLOCKING_RADIUS for m2, (x2, y2) in enumerate(solution) if m != m2):
                # line of sight is blocked
                continue
            score += single_score(x, y, px, py, pr.att_tastes[i][pr.mus_tastes[m]])
    return score

