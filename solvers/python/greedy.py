from src.utils import load_problem
import math

import json
from queue import PriorityQueue
import os
from dotenv import load_dotenv
from collections import defaultdict

BORDER_RADIUS = 10

load_dotenv()


def compute_score(x, y, px, py, taste):
    d2 = (x - px) ** 2 + (y - py) ** 2
    return int(math.ceil(10**6 * taste / d2))


class Task:
    def __init__(self, score, channel, x, y):
        self.score = score
        self.channel = channel
        self.x = x
        self.y = y

    def __lt__(self, other):
        return self.score < other.score

    def __repr__(self):
        return f"Task({self.score})@({self.x}, {self.y})"


class GreedySolver:
    def __init__(self, i):
        spec = load_problem(i)
        self.spec = spec
        self.i = i

    def _within_sector(self, x, y, px, py):
        dx = px - x
        dy = py - y
        sin_a = dy / math.sqrt(dx**2 + dy**2)
        # This boundary is inclusive: we consider the sound as blocked even if the intersection consists of one point
        return sin_a > 0.5

    def _compute_score(self, x, y, channel):
        total_score = 0
        for p in self.spec["attendees"]:
            px = p["x"]
            py = p["y"]
            if self._within_sector(x, y, px, py):
                total_score += compute_score(x, y, px, py, p["tastes"][channel])
        return total_score

    def _precompute(self, channel):
        y = (
            self.spec["stage_bottom_left"][1]
            + self.spec["stage_height"]
            - BORDER_RADIUS
        )
        x_stage_start = self.spec["stage_bottom_left"][0]
        x_stage_end = x_stage_start + self.spec["stage_width"]

        step = 1
        tasks = []
        for x in range(
            int(x_stage_start + BORDER_RADIUS),
            int(x_stage_end - BORDER_RADIUS + 1),
            step,
        ):
            score = self._compute_score(x=float(x), y=y, channel=channel)
            # print(f"{x:3} {score}")
            tasks.append(Task(channel=channel, score=score, x=x, y=y))
        return tasks

    def write_solution(self, assignment):
        # print(solution)
        placemenets = []
        for i in range(len(self.spec["musicians"])):
            x = assignment[i].x
            y = assignment[i].y
            placemenets.append({"x": float(x), "y": float(y)})
        output = {"placements": placemenets}
        user = os.environ["USER"]
        path = f"./solutions/{user}/{self.i}.json"
        print(f"Writing solution to {path}")
        with open(path, "w") as f:
            f.write(json.dumps(output, indent=2))

    def solve(self):
        assignment = {}
        count = 0

        ids_per_channel = defaultdict(list)
        for idx, channel in enumerate(self.spec["musicians"]):
            ids_per_channel[channel].append(idx)

        print(ids_per_channel)

        q = PriorityQueue()
        n_channels = len(self.spec["attendees"][0]["tastes"])
        for channel in range(n_channels):
            tasks = self._precompute(channel)
            for task in tasks:
                q.put((-task.score, task))

        while True:
            if q.empty():
                break
            if len(assignment) == len(self.spec["musicians"]):
                break
            score, cur = q.get()

            ids = ids_per_channel[cur.channel]
            # print(score, cur, ids)

            musician_id = None
            if len(ids) > 0:
                musician_id = ids[0]
            else:
                continue

            has_near = False
            for taken in assignment.values():
                if math.fabs(taken.x - cur.x) < 10:
                    has_near = True
                    break
            if not has_near:
                print(f"added {cur.score} at {cur.x} for musician {musician_id}")
                assert musician_id not in assignment
                assignment[musician_id] = cur
                ids.pop(0)

        print(assignment)
        self.write_solution(assignment)


def solve(i):
    print()
    print(f"Solving problem {i}")
    solver = GreedySolver(i)
    solver.solve()


if __name__ == "__main__":
    for i in range(22, 28):
        solve(i)
