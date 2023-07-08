


from src.utils import (Problem, BORDER_RADIUS, single_score, BLOCKING_RADIUS, NEARBY_MUSICIAN_RADIUS, dist_pt_line)
import math

import json
from queue import PriorityQueue
import os
from dotenv import load_dotenv
from collections import defaultdict
import sys

load_dotenv()

STEP = 10


class Task:
    def __init__(self, score, channel, x, y, visible):
        self.score = score
        self.channel = channel
        self.x = x
        self.y = y
        self.visible = visible

    def __lt__(self, other):
        return self.score < other.score

    def __repr__(self):
        return f"Task@({self.x}, {self.y})(score=({self.score})"

class GreedySolver2:
    def __init__(self, i):
        self.i = i
        self.spec = Problem(i)
        self.border = self._precompute_border_grid()
        # print (self.border)

    # visiblity is a list of visible attendees
    def _compute_score(self, x, y, channel, visible):
        total_score = 0
        for vis_idx in visible:
            a = self.spec.attendees[vis_idx]
            px = a[0]
            py = a[1]
            tastes = self.spec.att_tastes[vis_idx]
            total_score += single_score(x, y, px, py, tastes[channel])
        return total_score

    def _compute_visible(self, x, y, assignment):
        all_blocked = set()
        for task in assignment.values():
            blocked = self._compute_blocked(x, y, task.x, task.y)
            for b in blocked:
                all_blocked.add(b)

        visible = []
        for idx in range(len(self.spec.attendees)):
            if idx not in all_blocked:
                visible.append(idx)

        return visible
    
    def _compute_blocked(self, x, y, x2, y2):
        blocked = []
        for idx, a in enumerate(self.spec.attendees):
            # print (a)
            if dist_pt_line((x2, y2), (x, y), a) <= BLOCKING_RADIUS:
                blocked.append(idx)

        return blocked
    
    def _precompute_border_grid(self):
        result = []
        y_border_min = int(self.spec.stage_min_y + BORDER_RADIUS)
        y_border_max = int(self.spec.stage_max_y - BORDER_RADIUS)
        
        x_border_min = int(self.spec.stage_min_x + BORDER_RADIUS)
        x_border_max = int(self.spec.stage_max_x - BORDER_RADIUS)

        for x in range(
            x_border_min,
            x_border_max +1,
            STEP,
        ):
            result.append((x, y_border_min))
            result.append((x, y_border_max))

        for y in range(
            y_border_min,
            y_border_max +1,
            STEP,
        ):
            result.append((x_border_min, y))
            result.append((x_border_max, y))

        return list(sorted(set(result)))

    def _precompute_grid_tasks(self, assignment, ids_per_channel):
        tasks = []
        for (x, y) in self.border:
            has_near = False
            for task in assignment.values():
                if math.fabs(task.x - x) < NEARBY_MUSICIAN_RADIUS:
                    has_near = True
                    break
            if has_near:
                continue
        
            # attendes that visible from position (x, y)
            visible = self._compute_visible(x, y, assignment)

            # compute penalty scores for old musicians
            total_removed_score = 0
            for m_idx, task in assignment.items():
                blocked = self._compute_blocked(task.x, task.y, x, y)
                removed = set(b_idx for b_idx in blocked if b_idx in visible)
                removed_score = self._compute_score(x=float(x), y=y, channel=task.channel, visible=removed)
                total_removed_score += removed_score

            # update scores for new musician
            for channel in range(self.spec.ntastes):
                if len(ids_per_channel[channel]) == 0:
                    continue
                added_score = self._compute_score(x=float(x), y=y, channel=channel, visible=visible)
                # print(f"{x:3} {score}")
                tasks.append(Task(channel=channel, score=added_score - total_removed_score, x=x, y=y, visible=set(visible)))
            

        return tasks


    def solve(self):
        # map from musician id to placement (task)
        assignment = {}
        ids_per_channel = defaultdict(list)
        for idx, channel in enumerate(self.spec.mus_tastes):
            ids_per_channel[channel].append(idx)

        print(ids_per_channel)

        while True:
            # compute the grid of possible positions and scores, taking visibility into account
            # take the best position from grid
            # add to assignments
            # repeat
            if len(assignment) == self.spec.nmus:
                break
            tasks = self._precompute_grid_tasks(assignment, ids_per_channel)
            best_task = max(tasks, key = lambda x: x.score)
            # update old musicians visibility
            for task in assignment.values():
                blocked = self._compute_blocked(task.x, task.y, best_task.x, best_task.y)
                for b_idx in blocked:
                    if b_idx in task.visible:
                        task.visible.remove(b_idx)
                
            ids = ids_per_channel[best_task.channel]
            m_idx = ids.pop(0)
            assignment[m_idx] = best_task
            assert(best_task.score >= 0)
            print (f"[{len(assignment)}/{self.spec.nmus}] assign {m_idx} (ch={best_task.channel}) to {best_task}")
        
        print(assignment)
        self.write_solution(assignment)

    def write_solution(self, assignment):
        # print(solution)
        placemenets = []
        for i in range(len(assignment)):
            x = assignment[i].x
            y = assignment[i].y
            placemenets.append({"x": float(x), "y": float(y)})
        output = {"placements": placemenets}
        user = os.environ["USER"]
        path = f"./solutions/{user}/{self.i}.json"
        print(f"Writing solution to {path}")
        with open(path, "w") as f:
            f.write(json.dumps(output, indent=2))





def solve(i):
    print()
    print(f"Solving problem {i}")
    solver = GreedySolver2(i)
    solver.solve()


if __name__ == "__main__":
    solve(sys.argv[1])
