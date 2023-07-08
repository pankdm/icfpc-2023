
from src.utils import (Problem, BORDER_RADIUS, single_score, BLOCKING_RADIUS, NEARBY_MUSICIAN_RADIUS, dist_pt_line)
import math

import json
from queue import PriorityQueue
import os
from collections import defaultdict
import sys
from pprint import pprint
from random import randint


PREFIX = "random"

class RandomSolver:
    def __init__(self, i):
        self.i = i
        self.spec = Problem(i)

    def _write_solution(self, assignment):
        # print(solution)
        placemenets = []
        for i in range(len(assignment)):
            x = assignment[i][0]
            y = assignment[i][1]
            placemenets.append({"x": float(x), "y": float(y)})
        output = {"placements": placemenets}
       
        path = f"./solutions/{PREFIX}/{self.i}.json"
        print(f"Writing solution to {path}")
        with open(path, "w") as f:
            f.write(json.dumps(output, indent=2))        

    def solve(self):
        assignment = {}

        y_border_min = int(self.spec.stage_min_y + BORDER_RADIUS)
        y_border_max = int(self.spec.stage_max_y - BORDER_RADIUS)
        
        x_border_min = int(self.spec.stage_min_x + BORDER_RADIUS)
        x_border_max = int(self.spec.stage_max_x - BORDER_RADIUS)

        for i in range(self.spec.nmus):
            for j in range(10**5):
              x = randint(x_border_min, x_border_max)
              y = randint(y_border_min, y_border_max)
              is_good = True
              for (x1, y1) in assignment.values():
                  if (x - x1)**2 + (y - y1)**2 < NEARBY_MUSICIAN_RADIUS**2:
                      is_good = False
                      break
              if is_good:
                  assignment[i] = (x, y)
                  print (f"[{len(assignment)}/{self.spec.nmus}] assign {i} to ({x}, {y}) (iter = {j})")
                  break
                                      
            else:
                raise Exception("Haven't found a placement")
        self._write_solution(assignment)
            

def solve(i):
    print()
    print(f"Solving problem {i}")
    solver = RandomSolver(i)
    solver.solve()


if __name__ == "__main__":
    if sys.argv[1] == "range":        
        for i in range(1, 55):
            try:
                solve(i)
            except Exception as err:
                print(err)
    else:
        solve(sys.argv[1])
